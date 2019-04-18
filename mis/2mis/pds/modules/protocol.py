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
        self.dic = dict()  # dictionary

        # decode first
        if not isinstance(data, dict):
            data = codec.Bencodec.decode(data)

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
        return set(list(dic.keys())) == set(self.MESSAGE_TYPE[dic['type']]) and len(list(dic.keys())) == len(self.MESSAGE_TYPE[dic['type']])

    def _sort_and_store(self, dic) -> None:
        """Sort and store the given dictionary."""
        for key in sorted(dic):
            self.dic[key] = dic[key]

    def encode(self, encoding='utf-8') -> bytes:
        """Return an encoded Message in bencode in bytes-like object."""
        return bytes(codec.Bencodec.encode(self.dic).encode(encoding))

    @property
    def valid(self) -> bool:
        """Return True if the message is valid, False otherwise."""
        return self._valid


class Command:
    """RPC command."""

    # enumeration of supported commands
    # type : command (destination, <required parameters>)
    COMMAND_TYPE = {
        'message':    ('peer', ['command', 'dest', 'message', 'msg_from', 'msg_to']),
        'getlist':    ('peer', ['command', 'dest']),
        'peers':      ('peer', ['command', 'dest']),
        'reconnect':  ('peer', ['command', 'dest', 'reg_ipv4', 'reg_port']),
        'database':   ('node', ['command', 'dest']),
        'neighbors':  ('node', ['command', 'dest']),
        'connect':    ('node', ['command', 'dest', 'reg_ipv4', 'reg_port']),
        'disconnect': ('node', ['command', 'dest']),
        'sync':       ('node', ['command', 'dest'])
    }

    def __init__(self, data):
        """Command contructor."""
        self.dic = dict()  # dictionary

        # decode first
        if not isinstance(data, dict):
            data = codec.Bencodec.decode(data)

        # check the command
        self._valid = self._isvalid(data)

        # sort the dictionary by key
        if self.valid:
            self._sort_and_store(data)

    def __getitem__(self, key):
        """Return an item by the given key."""
        return self.dic.get(key) if self.dic is not None else None

    def __str__(self) -> str:
        """Return the string representation of the Command (JSON)."""
        return json.dumps(self.dic)

    def _isvalid(self, dic) -> bool:
        """Check whether the command is valid or not."""
        # invalid dictionary or unknown command type
        if dic is None or 'command' not in dic or dic['command'] not in Command.COMMAND_TYPE:
            return False

        # unknown target
        if 'dest' not in dic or dic['dest'] not in ['peer', 'node']:
            return False

        # given command has to be for a specific target
        if dic['dest'] != Command.COMMAND_TYPE[dic['command']][0]:
            return False

        # check command parameters
        for key, val in dic.items():
            if key not in self.COMMAND_TYPE[dic['command']][1] and val is not None:
                return False
            # store values
            else:
                if val is not None:
                    self.dic[key] = val

        # missing parameter
        for par in self.COMMAND_TYPE[dic['command']][1]:
            if dic.get(par, None) is None:
                return False

        # everything is OK
        return True

    def _sort_and_store(self, dic) -> None:
        """Sort and store the given dictionary."""
        for key in sorted(dic):
            self.dic[key] = dic[key]

    @property
    def valid(self) -> bool:
        """Return True if the command is valid, False otherwise."""
        return self._valid

    @staticmethod
    def get_supported_commands(target) -> str:
        """Get the supported commands for a given target."""
        supp = 'Supported commands:\n'
        if target == 'peer':
            supp += '    --peer --command message --from <username1> --to <username2> --message <content>\n' + \
                    '    --peer --command reconnect --reg-ipv4 <IP> --reg-port <port>\n' + \
                    '    --peer --command getlist\n' + \
                    '    --peer --command peers'
        elif target == 'node':
            supp += '    --node --command connect --reg-ipv4 <IP> --reg-port <port>\n' + \
                    '    --node --command disconnect\n' + \
                    '    --node --command neighbors\n' + \
                    '    --node --command database\n' + \
                    '    --node --command sync'
        else:
            supp += '    none for the given target: {}'.format(target)

        return supp

    def encode(self, encoding='utf-8') -> bytes:
        """Return an encoded Command in bencode in bytes-like object."""
        return bytes(codec.Bencodec.encode(self.dic).encode(encoding))
