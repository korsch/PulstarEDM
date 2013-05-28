#!/usr/bin/env python

import os
import sys
import glob
import logging
import tempfile
import shutil
from optparse import OptionParser


class SimulationRunner(object):

    @staticmethod
    def init_logger(debug=False):
        '''Sets the logging evel for the application.'''
        if debug:
            log_level=logging.DEBUG
        else:
            log_level=logging.WARNING

        logging.basicConfig(level=log_level)

    @staticmethod
    def setup_run_environment():
        '''Prepares the python environment and working directory for execution of
        simulations.

        Sets the current directory to the root directory of the project and adds
        the neccesary directories to sys.path so that all needed module can be
        founded. Checks to see if all needed folders are present, and creates
        them if they are not.
        '''

        #determine the base directory of the project and make that
        #the current directory
        projectDir = os.path.realpath(os.path.dirname(__file__)+"/..")
        os.chdir(projectDir)

        #Makes sure the results and logs directories exists

        for directory in ["results/", "logs/"]:
            if not os.path.isdir(directory):
                logging.debug("Need directory %s does not exist. Creating..." %
                        directory)
                os.mkdir(directory)


        #These modificaitons to the path are necessary to import the batch
        #and analysis scripts
        sys.path.append(projectDir)
        sys.path.append(projectDir+"/analysis")

    def __init__(self, opts, args):
        self.opts = opts
        self.args = args
        self.bulk = 0

    def __call__(self):
        self.run_sim()

    def run_sim(self):
        '''Does the actual work of running the simulation. '''
        import run_batch
        import do_analysis
        self.preprocess_parameters()
        self.oldstreams = (sys.stdout, sys.stderr)

        batch_template = self.generate_batch_template()

        for batch_args in self.generate_runs(batch_template):
            batch_args = self.allocate_tempfiles(batch_args)
            run_batch.run_batch(**batch_args)
            self.cleanup_run(batch_args)

        self.cleanup_sim()

        self.run_analysis()

    def configure_simulation(self):
        #Create macro file for this execution and use it to set parameters
        (configTmpFile, configTmpPath) = tempfile.mkstemp(suffix=".mac", dir=".")
        configFd = os.fdopen(configTmpFile, "w")
        print >> configFd, "/control/verbose"
        print >> configFd, "/nedm/detector/bulkAbsorption " + self.bulk +" cm"
        print >> configFd, "/nedm/detector/machinedLPB " + self.machined
        print >> configFd, "/nedm/detector/castLPB " + self.cast
        if opts.torlon_lpb:
            print >> configFd, "/nedm/detector/torlonLPB " + opts.torlon_lpb
        if(opts.length):
            print >> configFd, "/nedm/detector/halfLG_z " + self.length +" cm"
        
        if(opts.width):
            print >> configFd, "/nedm/detector/halfLG_x " + self.width + " cm"

        if opts.mirror_reflectivity:
            print >> configFd, "/nedm/detector/mirrorReflectivity " + opts.mirror_reflectivity

        if opts.mirror_type:
            print >> configFd, "/nedm/detector/mirrorType " + opts.mirror_type

        if(opts.macro):
            logging.debug("Using custom macros %s" % opts.macro)
            for macro in opts.macro:
                print >> configFd, "/control/execute %s" % macro
        else:
            logging.debug("Falling back to default random.mac")
            print >> configFd, "/control/execute g4macros/random.mac"

        print >> configFd, "/nedm/detector/update"

        if opts.commands:
            logging.debug("Appending extra commands")
            print >> configFd, self.commands

        configFd.close()

        return configTmpPath



    def generate_batch_template(self):
        batchArgs = {}

        if opts.events:
            batchArgs['events']=opts.events

        if opts.runs:
            batchArgs['iterations']=opts.runs
            
        return batchArgs

    def allocate_tempfiles(self, batchArgs):
        configTmpPath = self.configure_simulation()
        batchArgs['config_mac']=os.path.basename(configTmpPath)

        #Assure that this rootfile does not conflict with that of any other
        #simulation
        (rootTmpFile, rootTmpPath) = tempfile.mkstemp(suffix='.root', dir='.')
        os.close(rootTmpFile)
        batchArgs['rootFile'] = rootTmpPath

        return batchArgs



    def preprocess_parameters(self):
        #Output a selection of parameters. May want to add more here
        logging.debug("Options as supplied")
        for (k,v) in opts.__dict__.items():
            logging.debug(("%s =" % k) + str(v))

        if opts.commands:
            self.commands = '\n'.join([c.replace(';','\n') for c in opts.commands])
            logging.debug("Additional Commands supplied: ")
            for line in commands.splitlines():
                logging.debug("\t%s" % line)
            logging.debug("END COMMANDS")

        if opts.width:
            self.width = str(float(opts.width) * 2.54/2)
            logging.debug("Half Width: %s cm" % width)

        if opts.length:
            self.length = str(float(opts.length) * 2.54/2)
            logging.debug("Half Length: %s cm" % length)

        self.fileBase = "%s_%s_%s_%s" % tuple([opts.prefix] + args[0:3])
        (self.bulk, self.machined, self.cast) = args[0:3]

        logging.debug("Bulk: %s, Machined: %s, Cast: %s" % (self.bulk,
            self.machined, self.cast))


    def cleanup_run(self, batch_args):
        if opts.keep_rootfiles:
            logging.debug('copy rootfile to ' + batch_args['fname'].replace('.dat', '.root'))
            shutil.copy(batch_args['rootFile'], 'results/' +
            batch_args['fname'].replace('.dat', '.root'))

        os.remove(batch_args['config_mac'])
        os.remove(batch_args['rootFile'])

    def cleanup_sim(self):
        self.fileFolder = '/'.join(['data', opts.prefix, self.fileBase])
        rootFileFolder = '/'.join(['data', opts.prefix, 'rootfiles'])

        #Move datafiles to their subdirectories in the analysis/data/ folder
        if not os.path.isdir("analysis/" + self.fileFolder):
            os.makedirs("analysis/" + self.fileFolder)

        for fname in glob.glob("results/%s_*.dat"%self.fileBase):
            os.rename(fname, "analysis/" + self.fileFolder + "/" + os.path.basename(fname))

        if opts.keep_rootfiles:
            if not os.path.isdir('analysis/' + rootFileFolder):
                logging.debug('mkdir analysis/' + rootFileFolder)
                os.makedirs('analysis/' + rootFileFolder)
            for fname in glob.glob('results/%s_*.root' % self.fileBase):
                logging.debug('mv %s %s' % (fname,
                                           'analysis/' + rootFileFolder + '/'
                                           + os.path.basename(fname)))
                os.rename(fname, 'analysis/' + rootFileFolder + '/'
                          + os.path.basename(fname))

    def run_analysis(self):
        pass




#Currently broken
class LengthScan(SimulationRunner):

    def preprocess_parameters(self):
        super(SimulationRunner)
        #Load the lengths to simulate for
        self.lengthFile = file(opts.lf, "r")

    def generate_runs(self, batch_template):
        for inches in lengthFile:
            inches = inches.strip()
            cm = str(float(inches)*2.54/2)
            logging.debug("Inches: %s\tCm: %s" % (inches, cm))
            length_cmds = "/nedm/detector/halfLG_z " + cm +" cm\n" + "/nedm/detector/update\n"

            self.commands = length_cmds + self.commands


            #Datafile will contain the number of detected photons for each run
            dataFile = "%s_%sinches.dat" % (self.fileBase, inches)
            print "Generating " + dataFile

            #Redirect simulation and batch output to a log file
            logFile = open("logs/%s_%s.out"%(self.fileBase,inches),"w")
            (sys.stdout, sys.stderr)=(logFile,logFile)

            batchArgs={'fname': dataFile}
            batchArgs.update(batch_template)

            yield batchArgs
            
            (sys.stdout, sys.stderr)=self.oldstreams
            logFile.close()

        lengthFile.close()


    def run_analysis(self):
        #Begin analysis code
        os.chdir("analysis")

        #Pass data folder off to do_analysis script to determine attenuation length
        do_analysis.do_analysis(dataDir=self.fileFolder, fit=opts.do_fit)

class WidthScan(SimulationRunner):

    def preprocess_parameters(self):
        super(SimulationRunner)
        #Load the lengths to simulate for
        self.widthFile = file(opts.wf, "r")

    def generate_runs(self, batch_template):
        for inches in widthFile:
            inches = inches.strip()
            cm = str(float(inches)*2.54/2)
            logging.debug("Inches: %s\tCm: %s" % (inches, cm))

            #Datafile will contain the number of detected photons for each run
            dataFile = "%s_w%sinches.dat" % (self.fileBase, inches)
            print "Generating " + dataFile

            #Redirect simulation and batch output to a log file
            logFile = open("logs/%s_w%s.out"%(self.fileBase,inches),"w")
            (sys.stdout, sys.stderr)=(logFile,logFile)

            batchArgs={'fname': dataFile}
            batchArgs.update(batch_template)

            yield batchArgs
            
            (sys.stdout, sys.stderr)=self.oldstreams
            logFile.close()

        widthFile.close()

class NSimulations(SimulationRunner):

    def preprocess_parameters(self):
        super(NSimulations, self).preprocess_parameters()
        self.n = opts.n_simulations

    def generate_runs(self, batch_template):
        for i in range(self.n):
            dataFile = "%s_sim%d.dat" % (self.fileBase, i)
            print "Generating " + dataFile

            #Redirect simulation and batch output to a log file
            logFile = open("logs/%s_sim%d.out"%(self.fileBase, i),"w")
            (sys.stdout, sys.stderr)=(logFile,logFile)

            batchArgs = {'fname': dataFile}
            batchArgs.update(batch_template)

            yield batchArgs
            
            (sys.stdout, sys.stderr)=self.oldstreams
            logFile.close()



def init_parser():
    '''Initialize command line option parser. More can be found out by running scripts/run_sim.py --help. '''
    
    parser = OptionParser(usage="%prog [options] bulk machined cast")
    parser.add_option("-p","--prefix",dest="prefix",
                      default="testRun",
                      help="set the name prefix for datafiles")
    parser.add_option("-w","--width", dest="width",
                      help="set the width of the lightguide in inches.")
    parser.add_option("-l","--length",
                      help="set the length of the lightguide in inches.")
    parser.add_option("-W","--widths-file", dest="wf",
                      help="set the file that this script reads the width "
                      "points from. Paths are relative to the project "
                      "directory. Implicitly triggers a width scan.")
    parser.add_option("-L","--lengths-file", dest="lf",
                      help="set the file that this script reads the length "
                      "points from. Paths are relative to the project "
                      "directory. Implicitly enables a length scan.")
    parser.add_option("--debug", dest="debug",action="store_true",
                      help="turn on debug output")
    parser.add_option("-e","--events", dest="events", type="int",
                      help="set the number of events per run")
    parser.add_option("-r", "--runs", dest="runs", type="int",
                      help="set the number of runs per simulation.")
    parser.add_option('-n', '--n-simulations', type='int',
                      default=1, help="Number of times to run the simulation"
                      " macros. ")
    parser.add_option('--no-fit', action='store_false',
                      default=True,
                      dest='do_fit', help='disable fitting of data')
    parser.add_option('-m', '--macro', action='append',
                      help='supply additional configuration macros'
                      'to aid in setting up the system. They will be run'
                      'in the order they are given.')
    parser.add_option('-k', '--keep-rootfiles', action='store_true',
                      help='copy rootfile for each execution folder')
    parser.add_option('--torlon-lpb', help='set the lpb of the torlon plug')

    parser.add_option('--mirror-reflectivity',
            help="set the reflectivity of the rear reflector")

    parser.add_option('--mirror-type',
            help="set the type of the rear reflector")

    parser.add_option('-c', '--commands', action='append',
            help='Supply extra commands to be run by geant4 as part of the '
            'simulation setup. These are run after the specified macro files. '
            'Multiple -c options may be supplied.')

    return parser

if __name__ == '__main__':
    parser = init_parser()
    (opts, args) = parser.parse_args()

    SimulationRunner.init_logger(opts.debug)
    SimulationRunner.setup_run_environment()

    if len(args) < 3:
        parser.error("""Please supply the following parameters
        bulk - Bulk attenuation of acrlyic.
        machined - Loss per bounce on machined surface.
        cast - Loss per bounce on cast surface
    """)

    if opts.lf:
        sim_class = LengthScan
    elif opts.wf:
        sim_class = WidthScan
    else:
        sim_class = NSimulations

    sim = sim_class(opts, args)

    sim()

