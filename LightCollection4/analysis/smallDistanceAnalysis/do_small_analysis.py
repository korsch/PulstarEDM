#!/usr/bin/env python

import ROOT
import glob
import re

files = glob.glob('../rootfiles/mapSmall/*.root')
param_re = re.compile(r'([0-9.]+)_([0-9.]+)_([0-9.]+)')

ROOT.gROOT.SetStyle("Plain")
ROOT.gStyle.SetPalette(1)

tfiles = map(ROOT.TFile, files)
graphs = []

for tfile in tfiles:
    graph = tfile.Graph.Clone()
    params = param_re.search(tfile.GetPath()).groups()
    tfile.Close()
    graph.SetTitle('Bulk: {0}, Mach: {1}, Cast: {2}'.format(*params))
    graph.GetXaxis().SetTitle("Light Guide Length (inches)")
    graph.GetYaxis().SetTitle("PMT Hit Count")
    graph.GetYaxis().SetTitleOffset(1.25)
    graphs.append(graph)


