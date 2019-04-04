#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  Communication Protocol
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""

import json
import modules.codec as codec


class Message:
    """Peer to Peer protocol communication message."""

    # enumeration of supported communication message types
    # type : content (type and TID is implicit)
    MESSAGE_TYPE = {
        'hello':      ['type', 'txid', 'username', 'ipv4', 'port'],
        'getlist':    ['type', 'txid'],
        'list':       ['type', 'txid', 'peers'],
        'message':    ['type', 'txid', 'from', 'to', 'message'],
        'update':     ['type', 'txid', 'db'],
        'disconnect': ['type', 'txid'],
        'ack':        ['type', 'txid'],
        'error':      ['type', 'txid', 'verbose']
    }

    def __init__(self, data):
        """Message contructor."""
        # codec
        self.codec = codec.Bencodec

        # decode first
        if not isinstance(data, dict):
            data = self.codec.decode(data)

        # check the message
        self._valid = self._isvalid(data)

        # sort the dictionary by key
        if self.valid:
            self._sort_and_store(data)

    def __getitem__(self, key):
        """Return an item by the given key."""
        return self.dic.get(key) if self.dic is not None else None

    def __str__(self) -> str:
        """Return the string representation of the Message (JSON)."""
        return json.dumps(self.dic)

    def _isvalid(self, dic) -> bool:
        """Check whether the message is valid or not."""
        # invalid dictionary or not defined or unknown message type
        if dic is None or 'type' not in dic or dic['type'] not in Message.MESSAGE_TYPE:
            return False

        # given type has to have the same keys as defined by the protocol
        return set(list(dic.keys())) == set(self.MESSAGE_TYPE[dic['type']]) and \
               len(list(dic.keys())) == len(self.MESSAGE_TYPE[dic['type']])

    def _sort_and_store(self, dic) -> None:
        """Sort and store the given dictionary."""
        self.dic = dict()
        for key in sorted(dic):
            self.dic[key] = dic[key]

    def encode(self) -> str:
        """Return an encoded Message in bencode."""
        return self.codec.encode(self.dic)

    @property
    def valid(self) -> bool:
        """Return True if the message is valid, False otherwise."""
        return self._valid
