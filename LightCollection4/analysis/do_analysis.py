#!/usr/bin/env python
"""
A script for analysis of data directories produced by the simAttenuation.py script. Can be run from the command line with a directory as an argument or imported and used as a library.
"""


import sys
import os.path
import glob
import re
from array import array
from math import sqrt
from optparse import OptionParser

##@bug This options seems only to exist in 5.24.00/b and not 5.24.00
#import ROOT
#ROOT.PyConfig.IgnoreCommandLineOptions=True

from ROOT import TGraphErrors, TF1, TFile, gStyle


class MyFitFunc:
    """
    A class for creating exponential fit functions.
    
    Instantiation of the class creates and instance which may then
    be called to return a TF1 for fitting.
    """

    def __init__(self):
        self.fitFunc = TF1("fitFunc", "[0]*exp(-x/[1])", 0, 100)
        self.fitFunc.SetParName(0, "Constant")
        self.fitFunc.SetParName(1, "Attenuation Length")
        
    def __call__(self, guess):
        """
        Creates an an instance of a TF1 for fitting
        @param guess a guess for f(0)

        Returns a TF1 of an exponential decay suitable for fitting."""

        fitFunc = self.fitFunc.Clone()
        fitFunc.SetParameter(0, guess)
        fitFunc.SetParameter(1, 142)
        return fitFunc
    

def do_analysis(dataDir, fit=True):
    """
    @brief Does most of the work for the module
    @param dataDir A directory containing data files for each run
    @param fit Determine whether the resulting TGraph sould be fit
    @returns the TF1 used to fit the data

    The files in the directory passed to do_analysis must be in a very special format. The directory itself will have some name 'prefix_bulk_mach_cast' where bulk, mach, and cast are the bulk attenuation, machined loss per bounce, and cast loss per bounce of the simulation, respectively. Each file in the directory will have a name of the form 'prefix_bulk_mach_cast_ninches.dat' where n is the length of the lightguide in inches. Each file will contain k lines with each line containing the number of detected photons in run k.

    Do analysis will create a TGraphErrors object named Graph in a file named 'prefix_bulk_mach_cast.root which will be in the analysis/rootfiles directory.
    """
    print "Analysing: %s" % dataDir

    dataDir+="/"
    datFiles = glob.glob(os.path.normpath(dataDir+"*.dat"))

    lengths=[]
    totals=[]
    errs=[]
    lengthRe = re.compile("([0123456789.]*)inch")

    #We have to sort the files so that our data will be in the correct order:
    #TGraphErrors.Sort does not seem to work correctly on the cluster.
    datFiles.sort(key=lambda datFile : float(lengthRe.search(datFile).group(1)))

    for datFile in datFiles:
        length = lengthRe.search(datFile).group(1)
        f = file(datFile,"r")

        #Length of the LightGuide in inches
        l1=[float(i) for i in f.readlines()]

        #Number of runs
        n=float(len(l1))

        #Average photon count for this length
        av=sum(l1)/n

        f.close()
        lengths.append(float(length))
        totals.append(av)
        #We calculate the error assuming poisson statistics
        errs.append(sqrt(av/n))

        

    #We need to wrap our lists in python arrays so that they can be passed
    #by reference to PyROOT
    lengthArray = array('f', lengths)
    totalArray = array('f', totals)
    errArray = array('f', errs)
    n=len(lengths)
        
    tf = TFile("rootfiles/" +
               dataDir.strip('/').split('/')[-1] +
               ".root", "RECREATE")
    tf.cd()

    tg = TGraphErrors(n, lengthArray, totalArray,array('f', [0.0]*n), errArray)
    tg.SetTitle("Total Attenuation")
    tg.GetXaxis().SetTitle("Lightguide Length (in)")
    tg.GetYaxis().SetTitle("PMT Hit Count")
    
    if(fit):
        fitFactory = MyFitFunc()
        fitFunc = fitFactory(lengths[0])

        tg.Fit(fitFunc, "Q")

        print "Chisquare/NDF: %f" % (fitFunc.GetChisquare()/fitFunc.GetNDF())

        #The following includes a conversion from inches to cm.
        attLen = fitFunc.GetParameter(1)*2.54
        print("Attenuation length: " + str(attLen))
        err= fitFunc.GetParError(1)*2.54
        print("Err: " + str(err))
    else:
        fitFunc = None
        
    tg.Write()
    tf.Close()

    return fitFunc
    

if __name__=='__main__':
    #The directory to process is passed as the first argument
    parser = OptionParser(usage="%prog [options] data_directory")
    parser.add_option('--no-fit', action='store_false',
                      default=True,
                      dest='do_fit', help='disable fitting of data')
    (opts, args) = parser.parse_args()
    dataDir = args[0]
    do_analysis(dataDir, fit=opts.do_fit)
