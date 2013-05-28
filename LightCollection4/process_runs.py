#!/usr/bin/env python

import sys
from math import sqrt
from optparse import OptionParser

from ROOT import TFile, TH1F, TTree, gDirectory


def process_parameter(cut="det", resFile="runData.dat",
        rootFile="hist.root"):
    fdat = open("results/" + resFile, "a")

    ntot=0

    f1 = TFile.Open(rootFile, "READ")

    if not f1:
        print >> sys.stderr, "File not found"
    f1.ReadAll()
    runs = f1.GetNkeys()
    n = 0
        
    for (i, t_lg) in enumerate(list(f1.GetList())):
        n = t_lg.GetEntries()

        ndet = t_lg.Draw(cut, cut, "goff")
        
        print >> fdat, ndet
        print(" The number of photons satisfying %s for run %d is %d." % (cut, i, ndet))

        ntot += ndet

    f1.Close()
    fdat.close()

    navg = ntot/float(runs)
    nerr = sqrt(navg)/sqrt(runs)
    print("\n The average number of photons satisfying %s for run is: %f +/- %f"
            % (cut, navg, nerr))

    ravg = ntot/(float(n)*runs)
    rerr = sqrt(ravg/(float(n)*runs))
    print("\n Average percentage of photons satisfying %s: %4.2f%%+/-%4.2f%%" % 
            (cut, ravg*100.,rerr*100))


if __name__ == '__main__':
    parser = OptionParser(usage="%prog [options]")
    parser.add_option('--cut', help="set the cut to use", default="det")
    parser.add_option('--rootfile', help="the rootfile to process",
    default="hist.root")
    (opts, args) = parser.parse_args()

    process_parameter(opts.cut, "runData.dat", opts.rootfile)
