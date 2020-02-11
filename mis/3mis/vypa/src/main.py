#!/usr/bin/env python3
"""
- Project: VYPa19 language compiler
- Author: Tomas Aubrecht (xaubre02), Jan Kotras (xkotra01)
- Date:   2019-10-25
- Desc:   compiler.
"""

import sys
import argparse
from compiler import Compiler


def check_positive(value):
    ivalue = int(value)
    if ivalue <= 0:
        raise argparse.ArgumentTypeError("%s is an invalid positive int value" % value)
    return ivalue


def main():
    # compiler arguments
    arg_parser = argparse.ArgumentParser(description='Compiler for language \'VYPa19\'.', add_help=False)
    arg_parser.add_argument('-h', '--help', action='help', help='Display this help and exit.')
    arg_parser.add_argument('--silent', action='store_true', help='Activate the production mode')
    arg_parser.add_argument('--verbose', action='store_true', help='Activate the debugging mode of the interpreter')
    arg_parser.add_argument('--regs', type=check_positive, default=8, help='Configure the number of available general purpose registers')
    arg_parser.add_argument('--stack', type=check_positive, default=65535, help='Configure the size [words] of available stack')
    arg_parser.add_argument('input', metavar='INPUT', help='Specify the input file.')
    arg_parser.add_argument('output', metavar='OUTPUT', nargs='?', help='Specify the output file.')
    args = arg_parser.parse_args()

    # compile and exit
    compiler = Compiler()
    compiler.setup_compilation(args.silent, args.verbose, args.stack, args.regs)
    sys.exit(compiler.compile(args.input, args.output))


if __name__ == '__main__':
    main()
