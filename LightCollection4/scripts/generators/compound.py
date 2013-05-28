import re
import math
import logging

import ROOT

MAXDTWIST = 1.
MAXDAXIS = math.radians(1.)

class CubicVectorFunction:

    def __init__(self, a=(0, 0, 0), b=(0, 0, 0), c=(0, 0, 0), d=(0, 0, 0)):
        self.a = a
        self.b = b
        self.c = c
        self.d = d

    def evaluate(self, x):
        return tuple(a * x**3 + b*x**2 + c*x + d for (a, b, c, d)
                in zip(self.a, self.b, self.c, self.d))

class Slice:
    float_pattern = "-?\d*\.?\d*"
    vector_pattern = "\((?:\s*%s,?){%%d}\)" % float_pattern

    slice_re = re.compile("(%s)\s*(%s)\s*(%s)\s*(%s)" % 
                (vector_pattern % 3,
                vector_pattern % 3,
                vector_pattern % 2,
                float_pattern))

    def __init__(self, line=None):
        if line:
            (center, axis, dim, twist) = [self.parse(item) for item in
                    self.slice_re.match(line).groups()]
            self.center = ROOT.TVector3(*[float(f) for f in center])
            self.axis = ROOT.TVector3(*[float(f) for f in axis]).Unit()
            self.dim = ROOT.TVector2(*[float(f) for f in dim])
            self.twist = float(twist[0])

    def parse(self, string):
        return filter(lambda x : x != '', re.findall(self.float_pattern,
            string))

    def vector_to_string(self, vec):
        string = '(%f, %f' % (vec.X(), vec.Y())
        if type(vec) == ROOT.TVector3:
            string += ', %f' % vec.Z()
        string += ')'
        return string

    def __str__(self):
        components = map(self.vector_to_string, [self.center, self.axis, self.dim])
        components.append(self.twist)
        return "%s %s %s %f" % tuple(components)

    @classmethod
    def fit_path(klass, previous, current, n):
        difference = current.center - previous.center
        axis_i = previous.axis.Clone()
        axis_f = current.axis.Clone()
        
        d = (0., 0., 0.)
        c = tuple(axis_i(i) for i in range(3))
        b = tuple((3*(difference(i)) - (axis_f(i) + 2 * axis_i(i))*n)/float(n**2) for
                i in range(3))
        a = tuple( ( (axis_f(i) + axis_i(i))*n - 2*difference(i)) /float(n**3) for i in
                range(3))

        return CubicVectorFunction(a, b, c, d)





    @classmethod
    def interpolate(klass, previous, current, n, daxis, dtwist):
        center = previous.center.Clone()
        axis = previous.axis.Clone()
        twist = previous.twist

        difference = current.center.Clone()
        difference -= previous.center

        path_func = klass.fit_path(previous, current, n)
        
        interpolated = []
        for i in range(n-1):

            #twist this piece
            if abs(dtwist) > 0:
                twist += dtwist

            (pdx, pdy, pdz) = path_func.evaluate(i)
            (dx, dy, dz) = path_func.evaluate(i+1)
            new_center = center + ROOT.TVector3(dx, dy, dz)

            axis = ROOT.TVector3(dx - pdx, dy - pdy, dz - pdz).Unit()

            new_slice = Slice()
            new_slice.center = new_center.Clone()
            new_slice.axis = axis.Clone()
            new_slice.dim = previous.dim.Clone()
            new_slice.twist = twist

            interpolated.append(new_slice)


        interpolated.append(current)

        return interpolated
    

def generate(opts, args):
    if len(args) == 0:
        return
    fname = args[0]

    logging.info("Loading input file " + fname)
    data = file(fname ,'r')
    original_slices = [Slice(line) for line in data if not
            line.strip().startswith('#')]
    data.close

    previous = original_slices[0]

    logging.info("Processing Slices...\n\n")
    final_slices = [previous]

    for current in original_slices[1:]:
        dtwist = current.twist - previous.twist
        daxis_angle = abs(current.axis.Angle(previous.axis))
        difference = current.center - previous.center

        #Makes sure that the guide bends smoothly at vertices even
        #when its axis aligns with that of the next slice
        daxis_angle2 = abs(previous.axis.Angle(difference))

        if abs(dtwist) < MAXDTWIST and daxis_angle < MAXDAXIS \
                and daxis_angle2 < MAXDAXIS and opts.interpolate == 0:
            final_slices.append(current)
        else:
            logging.info("Interpolating")
            logging.debug("Interpolating between following slices:\n")
            logging.debug("Slice A: %s" % previous)
            logging.debug("Slice B: %s\n" % current)

            twist_n = 0
            if abs(dtwist) > 0:
                twist_n = abs(dtwist) / MAXDTWIST

            axis_n = 0
            axis_n2 = 0
            if daxis_angle > 0 or daxis_angle2 > 0:
                axis_n = daxis_angle / MAXDAXIS
                axis_n2 = daxis_angle2 / MAXDAXIS

            n = max(int(max(twist_n, axis_n, axis_n2, opts.interpolate)), 1)

            logging.info("Using %d steps\n" % n)

            daxis = daxis_angle / float(n)
            dtwist = dtwist / float(n)
            final_slices.extend(Slice.interpolate(previous, current, n, daxis,
                dtwist))
            
        previous = current

    logging.info("\nWriting to file %s..." % opts.output)
    output = file(opts.output, 'w')
    for s in final_slices:
        print >> output, s
    output.close

    logging.info("Done.")


