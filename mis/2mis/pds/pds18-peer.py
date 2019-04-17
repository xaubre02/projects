#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  Peer daemon
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""

import argparse
from modules import Peer


def main():
    # define arguments
    parser = argparse.ArgumentParser(description='Peer daemon of the hybrid P2P chat network.')
    req_args = parser.add_argument_group('Required arguments')
    req_args.add_argument('--id', metavar='<peer ID>', help='Unique ID of the peer on one host.', required=True)
    req_args.add_argument('--username', metavar='<user>', help='Unique username identifying this peer in the chat room.', required=True)
    req_args.add_argument('--chat-ipv4', metavar='<IPv4>', help='IPv4 address for the chat room connectivity.', required=True)
    req_args.add_argument('--chat-port', metavar='<port>', type=int, help='Port for the chat room connectivity. Allowed range: <0,65535>', required=True)
    req_args.add_argument('--reg-ipv4', metavar='<IPv4>', help='IPv4 address of the registration node.', required=True)
    req_args.add_argument('--reg-port', metavar='<port>', type=int, help='Port of the registration node. Allowed range: <0,65535>', required=True)

    # parse arguments
    args = parser.parse_args()

    # create a new peer
    peer = Peer(args.id, args.username, args.chat_ipv4, args.chat_port, args.reg_ipv4, args.reg_port)

    # start the peer
    peer.operate()


if __name__ == '__main__':
    main()
