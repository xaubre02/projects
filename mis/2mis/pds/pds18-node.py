#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  Node daemon
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""

import argparse
from modules.units import Node


def main():
    # define arguments
    parser = argparse.ArgumentParser(description='Node daemon of the hybrid P2P chat network.')
    req_args = parser.add_argument_group('Required arguments')
    req_args.add_argument('--id', metavar='<node ID>', help='Unique ID of the node on one host.', required=True)
    req_args.add_argument('--reg-ipv4', metavar='<IPv4>', help='IPv4 address of this registration node.', required=True)
    req_args.add_argument('--reg-port', metavar='<port>', type=int, help='Port of this registration node. Allowed range: <0,65535>', required=True)

    # parse arguments
    args = parser.parse_args()

    # create a new Node
    node = Node(args.id, args.reg_ipv4, args.reg_port)

    # start the node
    node.operate()


if __name__ == '__main__':
    main()
