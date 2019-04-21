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
import abc
import socket
import signal
import threading
from .protocol import Message, Command


# TODO: make txid unique for each message
class UnitRPC:
    """RPC interface."""

    # exceptions
    class BindError(Exception):
        """RPC socket bind error."""
        def __init__(self, msg):
            super().__init__('RPC socket error: ' + msg)

    SOCKET_PREFIX = 'xaubre02_pds18'  # unique prefix for naming the socket
    SOCKET_DIR = 'C:/Users/nxf42810/desktop'  # '/tmp'               # socket directory

    # destionations
    SOCKET_NODE = 'node'              # node
    SOCKET_PEER = 'peer'              # peer

    def __init__(self):
        """Initialize the RPC interface."""
        self._socket_rpc = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket_rpc_file = None

    def __del__(self):
        """Object destructor. Free all resources used by this unit."""
        if self._socket_rpc is not None:
            self._socket_rpc.close()
        
        if os.path.exists(self._socket_rpc_file):
            os.remove(self._socket_rpc_file)

    def rpc_init_socket_file(self, unit, unit_id) -> None:
        """Create a socket name from the given type of unit and it's ID."""
        sock_name = '{}_{}_{}'.format(UnitRPC.SOCKET_PREFIX, unit, unit_id)
        self._socket_rpc_file = os.path.join(UnitRPC.SOCKET_DIR, sock_name)

    def rpc_bind_socket(self, port):
        """Bind the RPC socket."""
        # socket name has to be initialized
        if self._socket_rpc_file is None:
            raise UnitRPC.BindError('uninitialized socket name')

        # check if socket exists
        if os.path.exists(self._socket_rpc_file):
            raise UnitRPC.BindError('socket already exists')
        
        self._socket_rpc.bind(('127.0.0.1', port))  # self._socket_rpc_file)
        self._socket_rpc.settimeout(0)

    def rpc_send_msg(self, msg) -> None:
        """Send a message."""
        pass

    def rpc_recv_msg(self) -> None:
        """Receive a message."""
        pass


class UnitUDP(UnitRPC):
    """Base class for other UDP communication units."""

    # size of the buffer on the receiving side
    RECV_BUFFER = 1024

    def __init__(self, unit, uid, reg_ipv4, reg_port):
        """Base constructor."""
        super().__init__()

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
        self._locks = dict()       # various thread locks
        self._txid = 0             # transaction ID

        self._locks['txid'] = threading.Lock()
        self._locks['timers'] = threading.Lock()
        self._locks['sock_reg'] = threading.Lock()

        # socket initialization
        self._socket_reg = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket_reg.settimeout(0)

        # catch SIGINT signal
        signal.signal(signal.SIGINT, self._sigint_handler)

        # initialize RPC socket
        try:
            self.rpc_init_socket_file(unit, uid)
            if unit == self.SOCKET_NODE:
                port = 12345 if uid == 'node1' else 12346
            else:
                port = 12347
            self.rpc_bind_socket(port)
        except UnitRPC.BindError:
            self._error('username already used')

        # initialize RPC command receiver
        self.rpc_receiver = threading.Thread(target=self.recv_commands)
        self.rpc_receiver.start()

    def __del__(self):
        """Object destructor. Free all resources used by this unit."""
        super().__del__()
        if self._socket_reg is not None:
            self._socket_reg.close()

        # RPC receiver
        self.rpc_receiver.join()

    def get_txid(self) -> int:
        """Get the txid and increment it."""
        self._locks['txid'].acquire()
        tmp = self._txid
        self._txid = (self._txid + 1) % 65535  # txid is ushort
        self._locks['txid'].release()
        return tmp

    def recv_commands(self) -> None:
        """Receive commands from the RPC application."""
        while self._run:
            try:
                data, addr = self._socket_rpc.recvfrom(UnitUDP.RECV_BUFFER)
                cmnd = Command(data)
                if not cmnd.valid:
                    self._notify('invalid RPC command received: {}'.format(str(data)), warning=True)
                else:
                    self.process_command(cmnd)
            except BlockingIOError:
                continue

    @abc.abstractmethod
    def process_command(self, cmnd) -> None:
        """Process a command from RPC application."""

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

    @staticmethod
    def acknowledge_msg(sock, txid, addr) -> None:
        """Acknowledge the message by its txid."""
        # acknowledge the message
        m = Message({'type': 'ack',
                     'txid': txid})
        sock.sendto(m.encode(), addr)

    def create_timer(self, name, duration, timeout_msg) -> None:
        """Wait for the acknowledge for the given transaction ID."""
        def notify():
            """Notify the user that the timer has finished."""
            self._notify('{}'.format(timeout_msg), warning=True)

        self._timers[name] = threading.Timer(duration, notify)
        self._timers[name].start()


class Node(UnitUDP):
    """Node of the hybrid P2P chat network."""

    def __init__(self, nid, ipv4, port):
        """Node constructor."""
        super().__init__(UnitUDP.SOCKET_NODE, nid, ipv4, port)  # call UnitUDP constructor

        # bind socket and set to non-blocking
        try:
            self._socket_reg.bind((self.reg_ipv4, self.reg_port))
            self._socket_reg.settimeout(0)
        except OSError:
            self._error('port already used')

        self._nodes = list()  # list of neighbors
        self._peers = list()  # list of connected peers

        self._accepting = True  # accepting UPDATE msgs flag

    def __del__(self):
        """Object destructor. Call all parents destructors."""
        super().__del__()

    def get_peer_list(self, node_ipv4=None, node_port=None) -> dict:
        """Get the list of all currently connected peers."""
        lst = dict()
        index = 0
        for peer in self._peers:
            # get only peers registered to the secified address
            if node_ipv4 is not None and node_port is not None and (peer.node_ipv4 != node_ipv4 or peer.node_port != node_port):
                continue

            rec = dict()
            rec['username'] = peer.username
            rec['ipv4'] = peer.ipv4
            rec['port'] = peer.port
            # save record
            lst[str(index)] = rec
            index += 1

        return lst

    def get_database(self) -> dict:
        """Get the peer database."""
        db = dict()
        keys = list()  # already processed keys
        for peer in self._peers:
            # create a key
            key = '{},{:d}'.format(peer.node_ipv4, peer.node_port)
            if key in keys:
                continue

            # save record
            db[key] = self.get_peer_list(peer.node_ipv4, peer.node_port)

        return db

    def is_my_peer(self, addr) -> bool:
        """Check whether the given address is from a peer registered to this node."""
        for peer in self._peers:
            if peer.ipv4 == addr[0] and peer.port == addr[1]:
                return True
        # non-registered peer
        return False

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
                self._locks['sock_reg'].acquire()
                data, addr = self._socket_reg.recvfrom(UnitUDP.RECV_BUFFER)
                self._locks['sock_reg'].release()
                mess = Message(data)
                if not mess.valid:
                    self._notify('invalid message received: {}'.format(str(data)), warning=True)
                else:
                    # ack message
                    if mess['type'] == 'ack':
                        # get the corresponding timer and stop it
                        timer = self._timers.get(str(mess['txid']), None)
                        if timer is not None:
                            timer.cancel()
                    # update message
                    elif mess['type'] == 'update':
                        # do not accept update messages
                        if not self._accepting:
                            continue
                        self.process_update(mess['db'], addr)
                    # hello message
                    elif mess['type'] == 'hello':
                        peer = PeerRecord(mess['username'], mess['ipv4'], mess['port'], self.reg_ipv4, self.reg_port, invalidate)
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
                    # getlist
                    elif mess['type'] == 'getlist':
                        # answer only to peers registered to this node
                        if self.is_my_peer(addr):
                            # acknowledge the message
                            self.acknowledge_msg(self._socket_reg, mess['txid'], addr)

                            # send the peer list
                            m = Message({'type': 'list',
                                         'txid': mess['txid'],
                                         'peers': self.get_peer_list()})
                            self._socket_reg.sendto(m.encode(), addr)

                            # wait for list acknowledge
                            self.create_timer(str(mess['txid']), 2, 'peer did not acknowledge list message')

                    else:
                        print("Received message: ", end='')
                        print(mess, end='')
                        print(' from: ', end='')
                        print(addr)
            # no datagrams available
            except BlockingIOError:
                self._locks['sock_reg'].release()
                continue

        # free resources
        for node in self._nodes:
            node.stop_timer()

        for peer in self._peers:
            peer.stop_timer()

    def print_nodes(self):
        """Print all known neighbors."""
        print('\n{:<16}  {:<5}'.format('IPv4', 'Port'))
        print('-' * 23)
        for node in self._nodes:
            print('{:<16}  {:<5d}'.format(node.ipv4, node.port))
        print()

    def print_peers(self):
        """Print all known peers."""
        print('\n{:<24}  {:<16}  {:<5}  {}'.format('Username', 'IPv4', 'Port', 'Authoritative'))
        print('-' * 65)
        for peer in self._peers:
            # authoritative records
            if peer.node_ipv4 == self.reg_ipv4 and peer.node_port == self.reg_port:
                auth = 'YES'
            else:
                auth = 'NO'
            print('{:<24}  {:<16}  {:<5d}  {}'.format(peer.username, peer.ipv4, peer.port, auth))
        print()

    def process_update(self, db, addr):
        """Process the database from UPDATE message."""
        def invalidate(node):
            """Remove a node from the database."""
            self._notify('node {}:{:d} has disconnected'.format(node.ipv4, node.port))
            timer_key = '{}:{:d}'.format(node.ipv4, node.port)
            # stop sending updates
            if self._timers[timer_key].is_alive():
                self._timers[timer_key].cancel()
            self._nodes.remove(node)

        def parse_key(k):
            """Parse the database key."""
            pos = k.find(',')
            return k[0:pos], int(k[pos+1:])

        # node record
        nr = NodeRecord(addr[0], addr[1], invalidate)
        # ignore updates from itself
        if nr.ipv4 == self.reg_ipv4 and nr.port == self.reg_port:
            return

        # store node record
        if nr not in self._nodes:
            self._nodes.append(nr)
            self._notify('new node has connected: {}:{:d}'.format(nr.ipv4, nr.port))
            # connect to it
            self.connect_and_maintain(nr.ipv4, nr.port)

        # update timer
        self._nodes[self._nodes.index(nr)].update()

        # remove all records from this node
        for peer in self._peers:
            if peer.node_ipv4 == nr.ipv4 and peer.node_port == nr.port:
                self._peers.remove(peer)

        for key, val in db.items():
            # parse the key
            ipv4, port = parse_key(key)
            # ignore records from this node
            if self.reg_ipv4 == ipv4 and self.reg_port == port:
                for _, peer in val.items():
                    p = PeerRecord(peer['username'], peer['ipv4'], peer['port'], nr.ipv4, nr.port, None)
                    self._peers.append(p)
            # authorative update records
            elif nr.ipv4 == ipv4 and nr.port == port:
                pass
            # non-authorative update records -> connect to the specified node
            else:
                self.connect_and_maintain(ipv4, port)

    def connect_and_maintain(self, ipv4, port):
        """connect to the specified node."""
        def send_update():
            """Send the UPDATE message to a node every 4 seconds."""
            # update message
            m = Message({'type': 'update',
                         'txid': self.get_txid(),
                         'db': self.get_database()})

            self._locks['sock_reg'].acquire()
            try:
                self._socket_reg.sendto(m.encode(), (ipv4, port))
            except Exception as ex:
                self._notify('failed to send the UPDATE message: {}'.format(str(ex)), warning=True)

            self._locks['sock_reg'].release()

            # repeat the function
            self._timers['{}:{:d}'.format(ipv4, port)] = threading.Timer(4, send_update)
            self._timers['{}:{:d}'.format(ipv4, port)].start()

        send_update()

    def disconnect(self):
        """Disconnect from the current network."""
        # set accepting flag to false
        self._accepting = False

    def synchronize(self):
        """Synchronize the database with other nodes."""
        db = self.get_database()
        # send updates to all known neighbors
        for node in self._nodes:
            # send update
            m = Message({'type': 'update',
                         'txid': self.get_txid(),
                         'db': db})

            self._locks['sock_reg'].acquire()
            self._socket_reg.sendto(m.encode(), (node.reg_ipv4, node.reg_port))
            self._locks['sock_reg'].release()

    def process_command(self, cmnd) -> None:
        """Process a command from RPC application."""
        # ignore
        if cmnd['dest'] == 'peer':
            return
        # database
        elif cmnd['command'] == 'database':
            self.print_peers()
        # connect
        elif cmnd['command'] == 'connect':
            self.connect_and_maintain(cmnd['reg_ipv4'], cmnd['reg_port'])
        # disconnect
        elif cmnd['command'] == 'disconnect':
            self.disconnect()
        # neighbors
        elif cmnd['command'] == 'neighbors':
            self.print_nodes()
        # sync
        elif cmnd['command'] == 'sync':
            self.synchronize()


class Peer(UnitUDP):
    """Peer of the hybrid P2P chat network."""

    def __init__(self, pid, username, chat_ipv4, chat_port, reg_ipv4, reg_port):
        """Peer constructor."""
        super().__init__(UnitUDP.SOCKET_PEER, pid, reg_ipv4, reg_port)  # call UnitUDP constructor

        # check if the chat IP address is valid
        try:
            socket.inet_aton(chat_ipv4)
            # inet_aton does accepts numbers as IP address
            if chat_ipv4.isdigit():
                raise OSError
        except OSError:
            self._socket_chat = None
            self._socket_msgs = None
            self._error('invalid chat address')

        self._username = username     # username
        self._chat_ipv4 = chat_ipv4   # chat IPv4 address
        self._chat_port = chat_port   # chat port
        self._peers = None

        # socket initialization (init, bind and set to non-blocking)
        self._socket_chat = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket_chat.bind((self.chat_ipv4, self.chat_port))
        self._socket_chat.settimeout(0)

        # socket lock
        self._locks['sock_chat'] = threading.Lock()
        self._got_list = threading.Event()

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

        self._got_list.clear()

    def connect_and_maintain(self) -> None:
        """Connect to a node in the P2P network and maintain the connection."""
        def send_hello():
            """Send the HELLO message to a node every 10 seconds."""
            # hello message
            hello = Message({'type': 'hello',
                             'txid': self.get_txid(),
                             'username': self.username,
                             'ipv4': self.chat_ipv4,
                             'port': self.chat_port})

            # send hello
            self._locks['sock_chat'].acquire()
            try:
                self._socket_reg.sendto(hello.encode(), (self.reg_ipv4, self.reg_port))
            except Exception as ex:
                self._notify('failed to send the HELLO message: {}'.format(str(ex)), warning=True)
            self._locks['sock_chat'].release()

            # repeat the function
            self._timers['hello'] = threading.Timer(10, send_hello)
            self._timers['hello'].start()

        # initialize
        send_hello()

    def disconnect(self) -> None:
        """Disconnect from the current node."""
        hello = Message({'type': 'hello',
                         'txid': self.get_txid(),
                         'username': self.username,
                         'ipv4': '0',
                         'port': 0})

        self._socket_reg.sendto(hello.encode(), (self.reg_ipv4, self.reg_port))

    def operate(self) -> None:
        """Start the peer functionality."""
        # connect to a node
        self.connect_and_maintain()

        while self._run:
            try:
                data, addr = self._socket_chat.recvfrom(UnitUDP.RECV_BUFFER)
                mess = Message(data)
                if not mess.valid:
                    self._notify('invalid message received {}'.format(str(data)), warning=True)
                else:
                    # message received
                    if mess['type'] == 'message':
                        print('{}:> {}'.format(mess['from'], mess['message']))
                    # ack received
                    elif mess['type'] == 'ack':
                        timer = self._timers.get(str(mess['txid']), None)
                        if timer is not None:
                            timer.cancel()
                    # list received
                    elif mess['type'] == 'list':
                        # TODO: what to do if the list message will not be received or received before getlist ack
                        self._peers = mess['peers']
                        self._got_list.set()  # synchronize
                        # acknowledge the message
                        self.acknowledge_msg(self._socket_reg, mess['txid'], addr)
                    else:
                        print(str(mess))

            # no datagrams available
            except BlockingIOError:
                continue

    def print_peers(self) -> None:
        """Print the peer list."""
        print('\n{:<24}  {:<16}  {:<5}'.format('User', 'IPv4', 'Port'))
        print('-' * 49)
        # no peer list
        if self._peers is None:
            return None

        for _, rec in self._peers.items():
            peer = PeerRecord(rec.get('username', None),
                              rec.get('ipv4', None),
                              rec.get('port', None),
                              None, None, None)
            # invalid peer record
            if not peer.is_valid():
                continue

            print('{:<24}  {:<16}  {:<5d}'.format(peer.username, peer.ipv4, peer.port))
        print()

    def get_list(self) -> None:
        """Get a list of currently connected peers and print the list."""
        txid = self.get_txid()
        m = Message({'type': 'getlist',
                     'txid': txid})

        self._got_list.clear()  # sync mechanism

        self._locks['sock_chat'].acquire()
        self._socket_chat.sendto(m.encode(), (self._reg_ipv4, self._reg_port))
        self._locks['sock_chat'].release()

        # wait for the acknowledge for 2 secs
        self.create_timer(str(txid), 2, 'getlist not acknowledged')
        # wait until the list is received (3 sec)
        self._got_list.wait(timeout=2)

    def get_addr_by_username(self, username) -> (str, int):
        """Get the address of the given username."""
        # no peer list
        if self._peers is None:
            return None

        for _, rec in self._peers.items():
            peer = PeerRecord(rec.get('username', None),
                              rec.get('ipv4', None),
                              rec.get('port', None),
                              None, None, None)
            # invalid peer record
            if not peer.is_valid():
                continue

            if peer.username == username:
                return peer.ipv4, peer.port

        # no such user
        return None

    def send_message(self, msg_from, msg_to, msg) -> None:
        """Send a message to an user."""
        # different usernames (from RPC and peer's)
        if self.username != msg_from:
            self._notify('different peer username vs. RPC command username: \'{}\' vs. \'{}\''.format(self.username, msg_from))
        # get peers
        self.get_list()

        # get the user's address
        addr = self.get_addr_by_username(msg_to)
        if addr is None:
            self._notify('no such username: \'{}\''.format(msg_to), warning=True)
            return

        # construct the message
        m = Message({'type': 'message',
                     'txid': self.get_txid(),
                     'from': msg_from,
                     'to': msg_to,
                     'message': msg})

        self._socket_chat.sendto(m.encode(), addr)

    def reconnect(self, ipv4, port) -> None:
        """Disconnect from the current node and connect to a new one."""
        # stop sending HELLO messages
        if self._timers['hello'].is_alive():
            self._timers['hello'].cancel()
        # disconnect from the current node
        self.disconnect()
        # change address
        self._reg_ipv4 = ipv4
        self._reg_port = port
        # connect to a new node
        self.connect_and_maintain()

    def process_command(self, cmnd) -> None:
        """Process a command from RPC application."""
        # ignore
        if cmnd['dest'] == 'node':
            return
        # getlist
        elif cmnd['command'] == 'getlist':
            self.get_list()
        # message
        elif cmnd['command'] == 'message':
            self.send_message(cmnd['msg_from'], cmnd['msg_to'], cmnd['message'])
        # peers
        elif cmnd['command'] == 'peers':
            self.get_list()
            self.print_peers()
        # reconnect
        elif cmnd['command'] == 'reconnect':
            self.reconnect(cmnd['reg_ipv4'], cmnd['reg_port'])


class NodeRecord:
    """Record of a node."""

    def __init__(self, ipv4, port, invalidate):
        """Record constructor."""
        self._ipv4 = str(ipv4)
        self._port = port
        self._err = ''
        self._inv_func = invalidate
        self._timer = None

    def __eq__(self, other) -> bool:
        """Equality function."""
        return self.ipv4 == other.ipv4 and \
               self.port == other.port

    def update(self) -> None:
        """Update the node record."""
        # restart timer
        self.start_timer()

    def start_timer(self) -> None:
        """Restart the validity timer."""
        # validity timer (12 seconds)
        self.stop_timer()
        self._timer = threading.Timer(12, self._inv_func, args=[self])
        self._timer.start()

    def stop_timer(self) -> None:
        """Stop the validity timer."""
        if self._timer is not None and self._timer.is_alive():
            self._timer.cancel()

    @property
    def ipv4(self) -> str:
        """Return the node's IPv4."""
        return self._ipv4

    @property
    def port(self) -> int:
        """Return the node's port."""
        return self._port


class PeerRecord:
    """Record of a peer."""

    def __init__(self, username, ipv4, port, node_ipv4, node_port, invalidate):
        """Record constructor."""
        self._username = username
        self._ipv4 = str(ipv4)
        self._port = port
        self._node_ipv4 = node_ipv4
        self._node_port = node_port
        self._err = ''
        self._inv_func = invalidate
        self._timer = None

    def __eq__(self, other) -> bool:
        """Equality function."""
        return self.username == other.username and \
               self.ipv4 == other.ipv4 and \
               self.port == other.port and \
               self.node_ipv4 == other.node_ipv4 and \
               self.node_port == other.node_port

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
    def node_ipv4(self) -> str:
        """Return the peer's registration node IPv4 address."""
        return self._node_ipv4

    @property
    def node_port(self) -> int:
        """Return the peer's registration node port."""
        return self._node_port

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
        self._socket_rpc.connect(('127.0.0.1', 12345))

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

        self._socket_rpc.send(cmd.encode())
