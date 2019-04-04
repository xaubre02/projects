#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  RPC
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""

import argparse


def main():
    # define arguments
    parser = argparse.ArgumentParser(description='RPC of the hybrid P2P chat network.')
    req_args = parser.add_argument_group('Required arguments')
    req_args.add_argument('', metavar='', help='', required=True)

    # parse arguments
    args = parser.parse_args()


if __name__ == '__main__':
    main()
