#!/usr/bin/env python

from optparse import OptionParser
from math import sqrt
from array import array

import ROOT


def relative_efficiency(base, other, blabel, olabel):
    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetPalette(1)
    
    base_file = ROOT.TFile(base)
    other_file = ROOT.TFile(other)
    base_g = base_file.Graph.Clone()
    other_g = other_file.Graph.Clone()

    base_file.Close()
    other_file.Close()

    base_g.SetMarkerStyle(ROOT.kFullTriangleUp)
    base_g.SetMarkerColor(ROOT.kRed)
    base_g.SetLineColor(ROOT.kRed)

    other_g.SetMarkerStyle(ROOT.kFullTriangleDown)
    other_g.SetMarkerColor(ROOT.kBlue)
    other_g.SetLineColor(ROOT.kBlue)

    tl = ROOT.TLegend(0.8, 0.8, 0.9, 0.9)
    tl.AddEntry(base_g, blabel or "No Hole", "P")
    tl.AddEntry(other_g, olabel or "With Hole", "P")

    n = base_g.GetN()
    x = base_g.GetX()
    base_y = base_g.GetY()
    other_y = other_g.GetY()
    base_ey = base_g.GetEY()
    other_ey = base_g.GetEY()

    for buf in [x, base_y, other_y, base_ey, other_ey]:
        buf.SetSize(n)

    div_y=[]
    div_ey=[]

    for by, oy, bey, oey in zip(base_y, other_y, base_ey, other_ey):
        div_y.append(oy/by)
        div_ey.append(div_y[-1]*sqrt( (oey/oy)**2 + (bey/by)**2))

        
    div_g = ROOT.TGraphErrors(n, x,  array('d', div_y),
                              array('d', [0.]*n), array('d', div_ey))

    div_g.SetTitle('Relative Transmission')
    div_g.GetXaxis().SetTitle(base_g.GetXaxis().GetTitle())
    div_g.GetYaxis().SetTitle('Ratio')

    axes = ROOT.TH2D('axes', 'PMT Hit Count vs Length',
            2, 0, base_g.GetXaxis().GetXmax(),
            2, other_g.GetYaxis().GetXmin(), base_g.GetYaxis().GetXmax())

    axes.GetXaxis().SetTitle(base_g.GetXaxis().GetTitle())
    axes.GetYaxis().SetTitle(base_g.GetYaxis().GetTitle())
    axes.GetYaxis().SetTitleOffset(1.2)

    ret_data = dict(base_g=base_g, other_g=other_g, 
                    legend=tl, ratio=div_g, axes=axes)

    def draw_comparison():
        ret_data['axes'].Draw()
        for obj in map(ret_data.__getitem__, "base_g other_g legend".split()):
            obj.Draw('P')
    ret_data['draw'] = draw_comparison
    return ret_data
    

if __name__=='__main__':
    parser = OptionParser()
    parser.add_option('--base-label',
                      help='Set the label for the first data set.')
    parser.add_option('--other-label',
                      help='Set the label for the second data set.')
    (opts, args) = parser.parse_args()
    t = relative_efficiency(*args, blabel=opts.base_label,
                       olabel=opts.other_label)
    ROOT.TPython.Prompt()
