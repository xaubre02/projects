#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  Node class
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""

import sys
import signal
from .protocol import Message


class Node:
    """Node of the hybrid P2P chat network."""

    def __init__(self, nid, ipv4, port):
        """Node constructor."""
        self._nid = nid    # node ID
        self._ipv4 = ipv4  # IPv4 address
        self._port = port  # port
        self._run = True   # node run flag

        # catch SIGINT signal
        signal.signal(signal.SIGINT, self._sigint_handler)

    @property
    def nid(self):
        """Return the node ID (NID)."""
        return self._nid

    @property
    def ipv4(self):
        """Return the IPv4 address of the node."""
        return self._ipv4

    @property
    def port(self):
        """Return port of the node."""
        return self._port

    def _cleanup(self):
        """Free all resources used by this node instance."""
        pass

    def _sigint_handler(self, signum, _) -> None:
        """SIGINT catch method."""
        print('\nABORTED: Signal {:d}'.format(signum), file=sys.stderr)
        self._run = False

    def operate(self):
        """Start the node functionality."""
        while self._run:
            pass

        self._cleanup()
