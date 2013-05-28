#!/usr/bin/env python

import sys
import subprocess
import os
import tempfile
import logging
from optparse import OptionParser

def run_batch(fname=None, events=10000, iterations=20, rootFile=None,
        config_mac=None, config_cmds=None, cut=None, executable="nedm"):

    processArgs={}

    if cut:
        processArgs['cut'] = cut

    (runTmpFile, runTmpPath)=tempfile.mkstemp(suffix=".mac",dir=".")
    batchRun = os.fdopen(runTmpFile, "w")
    print >> batchRun, "/run/beamOn %d" % events
    batchRun.close()

    (batchTmpFile, batchTmpPath)=tempfile.mkstemp(suffix=".mac",dir=".")
    batchMacro = os.fdopen(batchTmpFile, "w")
    if rootFile:
        processArgs['rootFile'] = rootFile
        print >> batchMacro, "/nedm/analysis/rootfile %s" % rootFile
    if config_mac:
        if not hasattr(config_mac, '__iter__'):
            config_mac = [config_mac]
        for mac in config_mac:
            print >> batchMacro, "/control/execute %s" % mac
    if config_cmds:
        print >> batchMacro, config_cmds
    print >> batchMacro, "/control/loop %s i 1 %d" % (os.path.basename(runTmpPath), iterations)
    batchMacro.close()
    subprocess.call(["%s/bin/%s/%s" % (os.environ['G4WORKDIR'],
                                       os.environ['G4SYSTEM'],
                                       executable),
                    os.path.basename(batchTmpPath)],
                    stdout=sys.stdout,
                    stderr=sys.stderr)
    os.remove(runTmpPath)
    os.remove(batchTmpPath)
    
    import process_runs
    if fname:
        processArgs['resFile'] = fname
    process_runs.process_parameter(**processArgs)

if __name__ == '__main__':

    parser = OptionParser(usage="%prog [options]")
    parser.add_option('--rootfile', help="set the name of rootfile generated.")
    parser.add_option('-f', '--datafile', help="set the name of the data file.")
    parser.add_option("-e","--events", dest="events", type="int",
                      help="set the number of events per run")
    parser.add_option("-r", "--runs", dest="runs", type="int",
                      help="set the number of runs per length.")
    parser.add_option("-m", "--macro", action='append',
                      help="set custom macro files to be run.")
    parser.add_option("-c", "--commands", action='append',
                      help="set custom commands to be at startup"
                      "these are executed after the config macro is"
                      "loaded.")
    parser.add_option("-x", "--executable",
                      help="set the executable to run as the simulation.")
    parser.add_option('--cut', help="set the cut to use", default="det")
    (opts, args) = parser.parse_args()

    run_args = {}
    if opts.datafile:
        run_args['fname'] = opts.datafile
    if opts.rootfile:
        run_args['rootFile'] = opts.rootfile
    if opts.events:
        run_args['events'] = opts.events
    if opts.runs:
        run_args['iterations'] = opts.runs
    if opts.macro:
        run_args['config_mac'] = opts.macro
    if opts.executable:
        run_args['executable'] = opts.executable
    if opts.commands:
        commands = '\n'.join([c.replace(';','\n') for c in opts.commands])
        logging.debug("Additional Commands supplied: ")
        for line in commands.splitlines():
            logging.debug("\t%s" % line)
        logging.debug("END COMMANDS")
        run_args['config_cmds'] = commands

    run_args['cut'] = opts.cut
                          
    run_batch(**run_args)



