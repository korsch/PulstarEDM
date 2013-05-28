#!/usr/bin/env python

import optparse
import sys
import os
import logging

import generators.compound as compound

callbacks = {}
callbacks['compound'] = compound.generate



def main(opts, args):
    fmt = '%(levelname)s:: %(message)s'
    level = logging.INFO
    if opts.verbose:
        level=logging.DEBUG
    elif opts.quiet:
        level=logging.WARNING
    elif opts.silent:
        level=logging.CRITICAL

    logging.basicConfig(level=level, format=fmt)

    if (len(args) >= 1):
        func = callbacks.get(args[0], None)
        if func:
            return func(opts, args[1:])
    parser.print_usage()
        

def init_parser():
    parser = optparse.OptionParser(usage='Usage: %prog [options] command'
    + '\n\nAvailable Commands:\n' + '\n'.join(callbacks.keys()))

    parser.add_option('-o', '--output', help="Specify filename of output file.",
            default='res/test.txt')

    parser.add_option('-q', '--quiet', help ="Minimize out output from this script.",
            action='store_true', default=False)

    parser.add_option('-v', '--verbose', help="Increase level of output.",
            action='store_true', default=False)

    parser.add_option('-s', '--silent', help="Turn off output",
            action='store_true', default=False)

    parser.add_option('-n', '--interpolate', help="Set minimum number " +
            "of interpolated steps between each pair of slices", default=0, type='int')

    return parser

if __name__ == '__main__':
    parser = init_parser()
    (opts, args) = parser.parse_args()
    main(opts, args)
