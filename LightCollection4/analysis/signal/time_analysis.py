#!/usr/bin/env python

import ROOT
import sys
import os
import glob
import re
from array import array
from math import sqrt

def process_time(rootFile):
    times_tree = ROOT.TNtuple("arrival_time", "Detection Time", "time")

    tf = ROOT.TFile(rootFile, "READ")
    tf.ReadAll()
    trees = list(tf.GetList())

    for run, tree in enumerate(trees):
        n = tree.Draw("time",
                "det==1", "goff")
        times = tree.GetV1()
        times.SetSize(n)
        for t in list(times):
            times_tree.Fill(t)

    tf.Close()

    return times_tree

if __name__ == '__main__':
    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetPalette(1)
    tree = process_time(sys.argv[1])
    ROOT.TPython.Prompt()


