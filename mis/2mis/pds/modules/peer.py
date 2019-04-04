#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  Peer class
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""

import sys
import signal
from .protocol import Message


class Peer:
    """Peer of the hybrid P2P chat network."""

    def __init__(self, pid, username, chat_ipv4, chat_port, reg_ipv4, reg_port):
        """Peer constructor."""
        self._pid = pid              # peer ID
        self._username = username    # username
        self._chat_ipv4 = chat_ipv4  # chat IPv4 address
        self._chat_port = chat_port  # chat port
        self._reg_ipv4 = reg_ipv4    # registration IPv4 address
        self._reg_port = reg_port    # registration port
        self._run = True             # peer run flag

        # catch SIGINT signal
        signal.signal(signal.SIGINT, self._sigint_handler)

    @property
    def pid(self):
        """Return the peer ID (PID)."""
        return self._pid

    @property
    def username(self):
        """Return the username of the peer."""
        return self._username

    @property
    def chat_ipv4(self):
        """Return the chat IPv4 address of the peer."""
        return self._chat_ipv4

    @property
    def chat_port(self):
        """Return the chat port of the peer."""
        return self._chat_port

    @property
    def reg_ipv4(self):
        """Return the registration IPv4 address of the peer."""
        return self._reg_ipv4

    @property
    def reg_port(self):
        """Return the registration port of the peer."""
        return self._reg_port

    def _cleanup(self):
        """Free all resources used by this peer instance."""
        pass

    def _sigint_handler(self, signum, _) -> None:
        """SIGINT catch method."""
        print('\nABORTED: Signal {:d}'.format(signum), file=sys.stderr)
        self._run = False

    def operate(self):
        """Start the peer functionality."""
        while self._run:
            pass

        self._cleanup()
