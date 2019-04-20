#!/usr/bin/env python3
"""
**************
***********************************
- Project: Hybrid P2P chat network
- Module:  Communication units
- Date:    2019-04-01
- Author:  Tomas Aubrecht
- Login:   xaubre02
***********************************
**************
"""

import os
import sys
import socket
import signal
import threading
from .protocol import Message, Command


class UnitRPC:
    """RPC interface."""

    # exceptions
    class BindError(Exception):
        """RPC socket bind error."""
        def __init__(self, msg):
            super().__init__('RPC socket error: ' + msg)

    SOCKET_PREFIX = 'xaubre02_pds18'  # unique prefix for naming the socket
    SOCKET_DIR = '/tmp'               # socket directory

    # destionations
    SOCKET_NODE = 'node'              # node
    SOCKET_PEER = 'peer'              # peer

    def __init__(self):
        """Initialize the RPC interface."""
        self._socket_rpc = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self._socket_rpc_file = None

    def __del__(self):
        """Object destructor. Free all resources used by this unit."""
        print('deleting UNITRPC')
        if self._socket_rpc is not None:
            self._socket_rpc.close()
        
        if os.path.exists(self._socket_rpc_file):
            os.remove(self._socket_rpc_file)

    def rpc_init_socket_file(self, unit, unit_id) -> None:
        """Create a socket name from the given type of unit and it's ID."""
        sock_name = '{}_{}_{}'.format(UnitRPC.SOCKET_PREFIX, unit, unit_id)
        self._socket_rpc_file = os.path.join(UnitRPC.SOCKET_DIR, sock_name)

    def rpc_bind_socket(self):
        """Bind the RPC socket."""
        # socket name has to be initialized
        if self._socket_rpc_file is None:
            raise UnitRPC.BindError('uninitialized socket name')

        # check if socket exists
        if os.path.exists(self._socket_rpc_file):
            raise UnitRPC.BindError('socket already exists')
        
        self._socket_rpc.bind(self._socket_rpc_file)

    def rpc_send_msg(self, msg) -> None:
        """Send a message."""
        pass

    def rpc_recv_msg(self) -> None:
        """Receive a message."""
        pass


class UnitUDP:
    """Base class for other UDP communication units."""

    # size of the buffer on the receiving side
    RECV_BUFFER = 1024

    def __init__(self, uid, reg_ipv4, reg_port):
        """Base constructor."""
        # check if the registration IP address is valid
        try:
            socket.inet_aton(reg_ipv4)
            # inet_aton does accepts numbers as IP address
            if reg_ipv4.isdigit():
                raise OSError
        except OSError:
            self._socket_reg = None
            self._error('invalid registration IPv4 address')

        self._uid = uid            # unit ID
        self._reg_ipv4 = reg_ipv4  # registration IPv4 address
        self._reg_port = reg_port  # registration port
        self._run = True           # node run flag
        self._timers = dict()      # various timers

        # socket initialization
        self._socket_reg = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # catch SIGINT signal
        signal.signal(signal.SIGINT, self._sigint_handler)

    def __del__(self):
        """Object destructor. Free all resources used by this unit."""
        print('deleting UnitUDP')
        if self._socket_reg is not None:
            self._socket_reg.close()

    @property
    def uid(self) -> str:
        """Return the unit ID (UID)."""
        return self._uid

    @property
    def reg_ipv4(self) -> str:
        """Return the registration IPv4 address of this unit."""
        return str(self._reg_ipv4)

    @property
    def reg_port(self) -> int:
        """Return the registration port of this unit."""
        return self._reg_port

    @staticmethod
    def _error(msg) -> None:
        """Print an error to console and exit with status -1."""
        print('Error: {}'.format(msg), file=sys.stderr)
        sys.exit(-1)

    @staticmethod
    def _notify(msg, warning=False) -> None:
        """Print a warning to console and exit with status -1."""
        prefix = 'Warning' if warning else 'Notice'
        print('{}: {}'.format(prefix, msg), file=sys.stderr)

    def _sigint_handler(self, signum, _) -> None:
        """SIGINT catch method."""
        print('\nABORTED: Signal {:d}'.format(signum), file=sys.stderr)
        self._run = False
        # cancel all timers
        for _, timer in self._timers.items():
            if timer is not None and timer.is_alive():
                timer.cancel()


class Node(UnitUDP, UnitRPC):
    """Node of the hybrid P2P chat network."""

    def __init__(self, nid, ipv4, port):
        """Node constructor."""
        UnitUDP.__init__(self, nid, ipv4, port)  # call UnitUDP constructor
        UnitRPC.__init__(self)                   # call UnitRPC constructor

        # bind socket and set to non-blocking
        try:
            self._socket_reg.bind((self.reg_ipv4, self.reg_port))
            self._socket_reg.settimeout(0)
        except OSError:
            self._error('port already used')

        self._peers = list()  # list of connected peers

        # initialize RPC socket
        try:
            self.rpc_init_socket_file(self.SOCKET_NODE, nid)
            self.rpc_bind_socket()
        except UnitRPC.BindError:
            self._error('username already used')

    def operate(self) -> None:
        """Start the node functionality."""
        def invalidate(invalid_peer, username=False):
            """Remove a peer record from the database."""
            # disconnected by user
            if username:
                for p in self._peers:
                    if p.username == invalid_peer:
                        p.stop_timer()
                        self._peers.remove(p)
                        self._notify('{} has disconnected'.format(invalid_peer))
            else:
                self._notify('{} has disconnected'.format(invalid_peer.username))
                self._peers.remove(invalid_peer)

        while self._run:
            try:
                data, addr = self._socket_reg.recvfrom(UnitUDP.RECV_BUFFER)
                mess = Message(data)
                if not mess.valid:
                    self._notify('invalid message received', warning=True)
                else:
                    # hello message
                    if mess['type'] == 'hello':
                        peer = PeerRecord(mess['username'], mess['ipv4'], mess['port'], invalidate)
                        # valid peer record
                        if peer.is_valid():
                            # remove the peer
                            if peer.ipv4.isdigit() and int(peer.ipv4) == 0 and peer.port == 0:
                                invalidate(peer.username, username=True)
                            # update the peer
                            elif peer in self._peers:
                                self._peers[self._peers.index(peer)].update(peer)
                            # add a new peer
                            else:
                                peer.start_timer()
                                self._peers.append(peer)
                                self._notify('new peer has connected: {}'.format(peer.username))
                        # invalid peer record
                        else:
                            err = Message({
                                'type': 'error',
                                'txid': 421,
                                'verbose': peer.error
                            })

                    else:
                        print("Received message: ", end='')
                        print(mess, end='')
                        print(' from: ', end='')
                        print(addr)
            # no datagrams available
            except BlockingIOError:
                continue

        for peer in self._peers:
            peer.stop_timer()


class Peer(UnitUDP, UnitRPC):
    """Peer of the hybrid P2P chat network."""

    # HELLO transaction ID
    TXID_HELLO = 420

    def __init__(self, pid, username, chat_ipv4, chat_port, reg_ipv4, reg_port):
        """Peer constructor."""
        super().__init__(pid, reg_ipv4, reg_port)

        # check if the chat IP address is valid
        try:
            socket.inet_aton(chat_ipv4)
            # inet_aton does accepts numbers as IP address
            if chat_ipv4.isdigit():
                raise OSError
        except OSError:
            self._socket_chat = None
            self._error('invalid chat IPv4 address')

        self._username = username     # username
        self._chat_ipv4 = chat_ipv4   # chat IPv4 address
        self._chat_port = chat_port   # chat port

        # socket initialization (init, bind and set to non-blocking)
        self._socket_chat = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket_chat.bind((self.chat_ipv4, self.chat_port))
        self._socket_chat.settimeout(0)

        # initialize RPC socket
        self.rpc_init_socket_file(self.SOCKET_NODE, pid)
        self.rpc_bind_socket()

    @property
    def username(self) -> str:
        """Return the username of the peer."""
        return self._username

    @property
    def chat_ipv4(self) -> str:
        """Return the chat IPv4 address of the peer."""
        return str(self._chat_ipv4)

    @property
    def chat_port(self) -> int:
        """Return the chat port of the peer."""
        return self._chat_port

    def __del__(self):
        """Object destructor. Free all resources used by peer."""
        super().__del__()
        if self._socket_chat is not None:
            self._socket_chat.close()

    def connect_and_maintain(self) -> None:
        """Connect to a node in the P2P network and maintain the connection."""
        def send_hello():
            """Send the HELLO message to a node every 10 seconds."""
            # send hello
            try:
                self._socket_reg.sendto(hello.encode(), (self.reg_ipv4, self.reg_port))
            except Exception as ex:
                self._notify('failed to send the HELLO message: {}'.format(str(ex)), warning=True)

            # repeat the function
            self._timers['hello'] = threading.Timer(10, send_hello)
            self._timers['hello'].start()

        # hello message
        hello = Message({'type': 'hello',
                         'txid': Peer.TXID_HELLO,
                         'username': self.username,
                         'ipv4': self.chat_ipv4,
                         'port': self.chat_port})

        # initialize
        send_hello()

    def disconnect(self) -> None:
        """Disconnect from the current node."""
        hello = Message({'type': 'hello',
                         'txid': Peer.TXID_HELLO,
                         'username': self.username,
                         'ipv4': '0',
                         'port': 0})

        self._socket_reg.sendto(hello.encode(), (self.reg_ipv4, self.reg_port))

    def operate(self) -> None:
        """Start the peer functionality."""
        # connect to a node
        self.connect_and_maintain()

        import time
        time.sleep(2)
        self.disconnect()

        while self._run:
            try:
                data, _ = self._socket_chat.recvfrom(UnitUDP.RECV_BUFFER)
                mess = Message(data)
                if not mess.valid:
                    self._error('invalid message received')
            # no datagrams available
            except BlockingIOError:
                continue


class PeerRecord:
    """Record of a peer."""

    def __init__(self, username, ipv4, port, invalidate):
        """Record constructor."""
        self._username = username
        self._ipv4 = str(ipv4)
        self._port = port
        self._err = ''
        self._inv_func = invalidate
        self._timer = None

    def __eq__(self, other) -> bool:
        """Equality function."""
        return self.username == other.username and self.ipv4 == other.ipv4 and self.port == other.port

    @property
    def username(self) -> str:
        """Return the peer's username."""
        return self._username

    @property
    def ipv4(self) -> str:
        """Return the peer's IPv4."""
        return self._ipv4

    @property
    def port(self) -> int:
        """Return the peer's port."""
        return self._port

    @property
    def error(self) -> str:
        """Return the error message."""
        return self._err

    def is_valid(self) -> bool:
        """Return True if the record is valid, False otherwise."""
        # check the IP address
        try:
            # inet_aton does accepts numbers as IP address
            if self.ipv4.isdigit() and int(self.ipv4) != 0:
                raise OSError

            socket.inet_aton(self.ipv4)
        except OSError:
            return False

        # check the port
        if not (isinstance(self.port, int) and 0 <= self.port <= 65535):
            return False

        return True

    def update(self, peer) -> None:
        """Update the peer record."""
        self._username = peer.username
        self._ipv4 = peer.ipv4
        self._port = peer.port
        # restart timer
        self.start_timer()

    def start_timer(self) -> None:
        """Restart the validity timer."""
        # validity timer (30 seconds)
        self.stop_timer()
        self._timer = threading.Timer(30, self._inv_func, args=[self])
        self._timer.start()

    def stop_timer(self) -> None:
        """Stop the validity timer."""
        if self._timer is not None and self._timer.is_alive():
            self._timer.cancel()


class RPC(UnitRPC):
    """RPC class."""

    def __init__(self, dest_unit, dest_id):
        """Initialize the RPC."""
        super().__init__()

        # initialize RPC socket name
        self.dest = dest_unit
        self.rpc_init_socket_file(dest_unit, dest_id)

        # try to connect to the created socket
        # try:
        # self._socket_rpc.connect(self.rpc_init_socket_file)

    def send_command(self, **params) -> None:
        """Send a command."""
        # process the given command
        cmd = Command(params)
        if not cmd.valid:
            # supported commands
            supp = Command.get_supported_commands(self.dest)
            # invalid command for a peer
            if self.dest == UnitRPC.SOCKET_PEER:
                print('Error: invalid command for a peer\n{}'.format(supp), file=sys.stderr)
            # invalid command for a node
            elif self.dest == UnitRPC.SOCKET_NODE:
                print('Error: invalid command for a node\n{}'.format(supp), file=sys.stderr)
            # invalid command (should not happen)
            else:
                print('Error: invalid command', file=sys.stderr)

            sys.exit(1)
