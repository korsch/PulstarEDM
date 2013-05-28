#!/usr/bin/env python

import ROOT
import sys
import os
import glob
import re
from array import array
from math import sqrt

def process_bounce(rootFile):
    tf = ROOT.TFile(rootFile, "READ")
    tf.ReadAll()
    trees = list(tf.GetList())
    runs = len(trees)
    cast_tot = 0.
    mach_tot = 0.
    tpb_tot = 0.

    for run, tree in enumerate(trees):
        n = tree.Draw("castBounces:machinedBounces:tpbBounces",
                "det==1", "goff")
        cast = tree.GetV1()
        machined = tree.GetV2()
        tpb = tree.GetV3()
        cast.SetSize(n)
        machined.SetSize(n)
        tpb.SetSize(n)
        cast_av = sum(cast)/float(n)
        mach_av = sum(machined)/float(n)
        tpb_av = sum(tpb)/float(n)
        cast_tot += cast_av
        mach_tot += mach_av
        tpb_tot += tpb_av

    tf.Close()
    mach_tot /= float(runs)
    cast_tot /= float(runs)
    tpb_tot /= float(runs)
    mach_err = sqrt(mach_tot/float(runs))
    cast_err = sqrt(cast_tot/float(runs))
    tpb_err = sqrt(tpb_tot/float(runs))

    return (mach_tot, cast_tot, tpb_tot, mach_err, cast_err, tpb_err)

if __name__ == '__main__':
    rootfile_dir = sys.argv[1]
    rootfiles = glob.glob(os.path.join(rootfile_dir,'*'))

    lengthre = re.compile("([0123456789.]*)inch")

    lengths = []
    n_m = []
    n_c = []
    n_t = []
    n_me = []
    n_ce = []
    n_te = []

    print('Length\tMach\tCast\tTPB')
    files = len(rootfiles)
    for rootFile in rootfiles:
        (m, c, t, m_e, c_e, t_e) = process_bounce(rootFile)
        length = float(lengthre.findall(os.path.basename(rootFile))[0])
        print("%2.2f\t%2.2f+/-%0.2f\t%2.2f+/-%0.2f\t%2.2f+/-%0.2f" % 
                (length, m, m_e, c, c_e, t, t_e))
        lengths.append(length)
        n_m.append(m)
        n_c.append(c)
        n_t.append(t)
        n_me.append(m_e)
        n_ce.append(c_e)
        n_te.append(t_e)

    bounce_data = ROOT.TFile('bounce_data.root', 'RECREATE')
    bounce_data.cd()
    dbl_array = lambda x : array('d', x)
    zeros = [0.]*files

    cast = ROOT.TGraphErrors(*[files] +
            map(dbl_array, [lengths, n_c, zeros, n_ce]))
    cast.SetName('cast')
    cast.SetTitle('Average Number of Cast Bounces')
    cast.GetYaxis().SetTitle('#bar{n}_{c}')

    mach = ROOT.TGraphErrors(*[files] +
            map(dbl_array, [lengths, n_m, zeros, n_me]))
    mach.SetName('mach')
    mach.SetTitle('Average Number of Machined Bounces')
    mach.GetYaxis().SetTitle('#bar{n}_{m}')

    tpb = ROOT.TGraphErrors(*[files] +
            map(dbl_array, [lengths, n_t, zeros, n_te]))
    tpb.SetName('tpb')
    tpb.SetTitle('Average Number of TPB Bounces')
    tpb.GetYaxis().SetTitle('#bar{n}_{tpb}')

    for t in (cast, mach, tpb):
        t.SetMarkerStyle(ROOT.kFullTriangleUp)
        t.SetMarkerColor(ROOT.kRed)
        t.SetLineColor(ROOT.kRed)
        t.GetXaxis().SetTitle('Lightguide Length (inches)')

    cast.Write()
    mach.Write()
    tpb.Write()
    bounce_data.Close()
