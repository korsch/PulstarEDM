#!/usr/bin/env python

import sys
from array import array
from math import sqrt

import ROOT

def generate_plot(rootfile, dim='z'):
    tf = ROOT.TFile(rootfile)
    tf.ReadAll()
    trees = list(tf.GetList())
    positions = []
    ratios = []
    errs = []

    for tree in trees:
        ndet = tree.Draw('', 'tpb==1', 'goff')
        n = tree.GetEntries()
        ratio = float(ndet)/n
        err = sqrt(ratio/float(n)) 
        ratios.append(ratio)
        errs.append(err)
        tree.GetEntry(0)
        pos = {'x' : tree.x , 'y' : tree.y, 'z' : tree.z}[dim]
        positions.append(pos)

    m = len(trees)

    tf.Close()
    tg = ROOT.TGraphErrors(m, *map(lambda x : array('d', x),
        [positions, ratios, [0]*m, errs]))

    tg.GetXaxis().SetTitle(dim.upper() + ' Position (cm)')
    tg.GetYaxis().SetTitle('Fraction of light incident on TPB')
    tg.GetYaxis().SetTitleOffset(1.2)
    tg.SetTitle('TPB Coverage')
    tg.SetMarkerColor(ROOT.kRed)
    tg.SetLineColor(ROOT.kRed)
    tg.SetMarkerStyle(ROOT.kFullTriangleUp)
    return tg


if __name__ == '__main__':
    args=sys.argv[1:]
    tg = generate_plot(*args)
    ROOT.gROOT.SetStyle('Plain')
    ROOT.gStyle.SetPalette(1)
    ROOT.TPython.Prompt()
