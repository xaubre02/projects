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
    req_args.add_argument('--id', metavar='<app ID>', help='Unique ID of the peer/node on one host.', required=True)
    req_args.add_argument('--command', metavar='<command>', help='Command for the specified peer/node.', required=True)

    exc_args = parser.add_mutually_exclusive_group(required=True)
    exc_args.add_argument('--node', help='Connect to the node.')
    exc_args.add_argument('--peer', help='Connect to the peer.')

    opt_args = parser.add_argument_group('Optional arguments')
    opt_args.add_argument('--reg-ipv4', metavar='<IPv4>', help='IPv4 address of the registration node.')
    opt_args.add_argument('--reg-port', metavar='<port>', type=int, help='Port of the registration node. Allowed range: <0,65535>')
    opt_args.add_argument('--from', metavar='<username1>', help='User who will send a message.')
    opt_args.add_argument('--to', metavar='<username2>', help='User who will receive a message.')
    opt_args.add_argument('--message', metavar='<message>', help='A message to be send to the specified user.')

    # parse arguments
    args = parser.parse_args()

    # parse arguments
    args = parser.parse_args()


if __name__ == '__main__':
    main()
