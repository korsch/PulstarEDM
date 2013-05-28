#!/usr/bin/env python
import os
import sys
import glob
import re
from optparse import OptionParser

from array import array
from math import sqrt

import ROOT

def process_detected(rootFile, cut='det==1'):
    tf = ROOT.TFile(rootFile, "READ")
    tf.ReadAll()
    trees = list(tf.GetList())
    runs = len(trees)
    ravg = 0
    n = 0
    
    for tree in trees:
        ndet = tree.Draw('', cut, 'goff') 
        n = tree.GetEntries()
        ratio = float(ndet)/float(n)
        ravg += ratio

    ravg /= float(runs)
    rerr = sqrt(ravg/(float(runs)*float(n)))

    return (ravg, rerr)


def main(opts, args):
    rootfile_dir = args[0]
    rootfiles = glob.glob(os.path.join(rootfile_dir,'*'))

    lengthre = re.compile("([0123456789.]*)inch")

    files = len(rootfiles)
    ratios = []
    errs = []
    lengths = []

    for rootFile in rootfiles:
        length = float(lengthre.findall(os.path.basename(rootFile))[0])
        (ratio, err) = process_detected(rootFile, opts.cut)
        ratios.append(ratio)
        errs.append(err)
        lengths.append(length)

    data_fname = opts.o or 'geom_data.root'
    geom_data = ROOT.TFile(data_fname, 'RECREATE')
    graph = ROOT.TGraphErrors(files, array('d',lengths),
        array('d', ratios), array('d',[0]*files), array('d', errs))
    if opts.w:
        graph.GetXaxis().SetTitle('Lightguide Width (inches)')
    elif opts.l:
        graph.GetXaxis().SetTitle('Lightguide Lenght (inches)')

    graph.SetMarkerStyle(ROOT.kFullTriangleUp)
    graph.SetMarkerColor(ROOT.kRed)
    graph.SetLineColor(ROOT.kRed)
    graph.Write()
    geom_data.Close()


if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option('-o', help='Set output filename')
    parser.add_option('-l', action='store_true',
            help='Identify as a length scan')
    parser.add_option('-w', action='store_true',
            help='Identify as a width scan')
    parser.add_option('--cut', default='det==1',
            help='Set the cut for the ratio')
    (opts, args) = parser.parse_args()
    main(opts, args)
