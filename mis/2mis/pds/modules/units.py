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


class UnitRPC:
    """RPC interface."""

    # exception
    class BindError(Exception):
        """RPC socket bind error."""
        def __init__(self, msg):
            super().__init__('RPC socket error: ' + msg)

    SOCKET_PREFIX = 'xaubre02_pds18'  # unique prefix for naming the socket
    SOCKET_DIR = '/tmp'               # socket directory

    # targets
    SOCKET_NODE = 'node'              # node
    SOCKET_PEER = 'peer'              # peer

    def __init__(self):
        """Initialize the RPC interface."""
        self._socket_rpc = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self._socket_rpc_file = None

    def __del__(self):
        """Object destructor. Free all resources used by this unit."""
        # close socket
        if self._socket_rpc is not None:
            self._socket_rpc.close()

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
            self._socket_rpc_file = None
            raise UnitRPC.BindError('ID already exists')
        
        self._socket_rpc.bind(self._socket_rpc_file)
        self._socket_rpc.settimeout(0)


class UnitUDP(UnitRPC):
    """Base class for other UDP communication units."""

    # size of the buffer on the receiving side
    RECV_BUFFER = 4096

    def __init__(self, unit, uid, reg_ipv4, reg_port):
        """Base constructor."""
        super().__init__()

        # receiver socket declaration
        self.rpc_receiver = None

        # check if the registration IP address is valid
        try:
            socket.inet_aton(reg_ipv4)
            # inet_aton does accepts numbers as IP address
            if reg_ipv4.isdigit():
                raise OSError
        except OSError:
            self._socket_reg = None
            self._error('invalid registration IPv4 address')

        # check the port
        if not (0 <= reg_port <= 65535):
            self._socket_reg = None
            self._error('invalid port value: out of range <0,65535>')

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
            self.rpc_bind_socket()
        except UnitRPC.BindError:
            self._error('{}\'s ID already used'.format(unit))

        # initialize RPC command receiver
        self.rpc_receiver = threading.Thread(target=self.recv_commands)
        self.rpc_receiver.start()

    def __del__(self):
        """Object destructor. Free all resources used by this unit."""
        super().__del__()
        if self._socket_reg is not None:
            self._socket_reg.close()

        # RPC receiver
        if self.rpc_receiver is not None:
            self.rpc_receiver.join()
        
        # delete socket file
        if self._socket_rpc_file is not None and os.path.exists(self._socket_rpc_file):
            os.remove(self._socket_rpc_file)

    def get_txid(self) -> int:
        """Get the txid and increment it."""
        self._locks['txid'].acquire()  # lock
        tmp = self._txid
        self._txid = (self._txid + 1) % 65535  # txid is ushort
        self._locks['txid'].release()  # unlock
        return tmp

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

    def _error(self, msg) -> None:
        """Print an error to console and exit with status -1."""
        print('Error: {}'.format(msg), file=sys.stderr)
        self._run = False
        sys.exit(-1)

    @staticmethod
    def _notify(msg, warning=False) -> None:
        """Print a warning to console and continue with the program execution."""
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

    @staticmethod
    def send_error_msg(sock, txid, verbose, addr) -> None:
        """Send an error message."""
        # Error message
        m = Message({'type': 'error',
                     'txid': txid,
                     'verbose': verbose})
        sock.sendto(m.encode(), addr)

    def create_timer(self, name, duration, timeout_msg) -> None:
        """Wait for the acknowledge for the given transaction ID."""
        def notify():
            """Notify the user that the timer has finished."""
            self._notify('{}'.format(timeout_msg), warning=True)

        self._timers[name] = threading.Timer(duration, notify)
        self._timers[name].start()

    @abc.abstractmethod
    def operate(self) -> None:
        """Start the unit functionality."""

    @abc.abstractmethod
    def process_command(self, cmnd) -> None:
        """Process a command from RPC application."""

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


class Node(UnitUDP):
    """Node of the hybrid P2P chat network."""

    def __init__(self, nid, ipv4, port):
        """Node constructor."""
        self._nodes = list()  # list of neighbors
        self._peers = list()  # list of connected peers

        super().__init__(UnitUDP.SOCKET_NODE, nid, ipv4, port)  # call UnitUDP constructor

        # bind socket and set to non-blocking
        try:
            self._socket_reg.bind((self.reg_ipv4, self.reg_port))
            self._socket_reg.settimeout(0)
        except OSError:
            self._error('port already used')

        self._accepting = True  # accepting UPDATE msgs flag

    def __del__(self):
        """Object destructor. Call all parents destructors."""
        self.disconnect()
        super().__del__()

    def get_peer_list(self, node_ipv4=None, node_port=None) -> dict:
        """Get the list of all currently connected peers."""
        lst = dict()
        index = 0
        for peer in self._peers:
            # get only peers registered to the secified node(address)
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

            # save record with all peers registered to this node
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
            # HELLO timeout
            else:
                self._notify('{} has disconnected'.format(invalid_peer.username))
                self._peers.remove(invalid_peer)
            self.synchronize()  # inform other nodes

        while self._run:
            try:
                self._locks['sock_reg'].acquire()
                data, addr = self._socket_reg.recvfrom(UnitUDP.RECV_BUFFER)
                self._locks['sock_reg'].release()
                # ignore messages from itself
                if addr[0] == self.reg_ipv4 and addr[1] == self.reg_port:
                    continue

                mess = Message(data)
                if not mess.valid:
                    self._notify('invalid message received: {}'.format(str(data)), warning=True)
                    self._locks['sock_reg'].acquire()
                    self.send_error_msg(self._socket_reg, 0, 'couldn\'t decode the message: {}'.format(str(data)), addr)
                    self._locks['sock_reg'].release()
                else:
                    # ack message
                    if mess['type'] == 'ack':
                        # get the corresponding timer and stop it
                        timer = self._timers.get(str(mess['txid']), None)
                        if timer is not None:
                            timer.cancel()
                    # update message
                    elif mess['type'] == 'update':
                        # node is accepting update messages
                        if self._accepting:
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
                                self.synchronize()  # inform other nodes
                        # invalid peer record
                        else:
                            self._locks['sock_reg'].acquire()
                            self.send_error_msg(self._socket_reg, mess['txid'], 'invalid peer data', addr)
                            self._locks['sock_reg'].release()
                    # getlist
                    elif mess['type'] == 'getlist':
                        # answer only to peers registered to this node
                        if self.is_my_peer(addr):
                            # message
                            txid = self.get_peer_list()
                            m = Message({'type': 'list',
                                         'txid': mess['txid'],
                                         'peers': txid})
                            self._locks['sock_reg'].acquire()
                            # acknowledge the message
                            self.acknowledge_msg(self._socket_reg, mess['txid'], addr)
                            # send the peer list
                            self._socket_reg.sendto(m.encode(), addr)
                            self._locks['sock_reg'].release()

                            # wait for list acknowledge
                            self.create_timer(str(mess['txid']), Message.ACK_WAIT, 'peer did not acknowledge LIST message with TXID={}'.format(str(txid)))
                    # disconnect
                    elif mess['type'] == 'disconnect':
                        self.process_disconnect(addr)
                        # acknowledge the disconnect
                        self._locks['sock_reg'].acquire()
                        self.acknowledge_msg(self._socket_reg, mess['txid'], addr)
                        self._locks['sock_reg'].release()
                    # an error received
                    elif mess['type'] == 'error':
                        self._notify('an error message received: {}'.format(mess['verbose']), warning=True)
                    # unknown message
                    else:
                        self._notify('unsupported message type received: {}'.format(mess['type']), warning=True)
            # no datagrams available
            except BlockingIOError:
                self._locks['sock_reg'].release()
                continue

        # free resources
        for node in self._nodes:
            node.stop_timer()

        for peer in self._peers:
            peer.stop_timer()

    def print_nodes(self) -> None:
        """Print all known neighbors."""
        print('\n{:<16}  {:<5}'.format('IPv4', 'Port'))
        print('-' * 23)
        for node in self._nodes:
            print('{:<16}  {:<5d}'.format(node.ipv4, node.port))
        print()

    def print_peers(self) -> None:
        """Print all known peers."""
        print('\n{:<24}  {:<16}  {:<5}  {:<13}  {}'.format('Username', 'IPv4', 'Port', 'Authoritative', 'Registrar'))
        print('-' * 81)
        for peer in self._peers:
            # authoritative records
            if peer.node_ipv4 == self.reg_ipv4 and peer.node_port == self.reg_port:
                auth = 'YES'
            else:
                auth = 'NO'
            print('{:<24}  {:<16}  {:<5d}  {:<13}  {}:{:d}'.format(peer.username, peer.ipv4, peer.port, auth, peer.node_ipv4, peer.node_port))
        print()

    def process_update(self, db, addr) -> None:
        """Process the database from UPDATE message."""
        def invalidate(node):
            """Remove a node from the database."""
            self._notify('node {}:{:d} has disconnected'.format(node.ipv4, node.port))
            timer_key = '{}:{:d}'.format(node.ipv4, node.port)
            # stop sending updates
            if self._timers[timer_key].is_alive():
                self._timers[timer_key].cancel()
            # remove all peers registered to this address
            peers_to_remove = list()
            for p_old in self._peers:
                # remove peer
                if p_old.node_ipv4 == addr[0] and p_old.node_port == addr[1]:
                    peers_to_remove.append(p_old)
            for p_old in peers_to_remove:
                if p_old in self._peers:
                    self._peers.remove(p_old)
            # remove the node itself
            self._nodes.remove(node)

        def parse_key(k) -> (str, int):
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

        to_remove = list()
        # remove all records from this node
        for peer in self._peers:
            if peer.node_ipv4 == nr.ipv4 and peer.node_port == nr.port:
                to_remove.append(peer)
        
        for peer in to_remove:
            if peer in self._peers:
                self._peers.remove(peer)

        for key, val in db.items():
            # parse the key
            ipv4, port = parse_key(key)
            # ignore records from this node
            if self.reg_ipv4 == ipv4 and self.reg_port == port:
                continue
            # authorative update records
            elif nr.ipv4 == ipv4 and nr.port == port:
                for _, peer in val.items():
                    # append a new peer
                    p = PeerRecord(peer['username'], peer['ipv4'], peer['port'], nr.ipv4, nr.port, None)
                    if p not in self._peers:
                        self._peers.append(p)
            # non-authorative update records
            else:
                # connect to the specified node if did not in the past already
                if NodeRecord(ipv4, port, None) not in self._nodes:
                    self.connect_and_maintain(ipv4, port)

    def connect_and_maintain(self, ipv4, port) -> None:
        """Connect to the specified node and maintain the connection."""
        # do not connect to itself
        if ipv4 == self.reg_ipv4 and port == self.reg_port:
            return

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

            # repeat the function (use timer)
            key = '{}:{:d}'.format(ipv4, port)
            if key in self._timers and self._timers[key].is_alive():
                self._timers[key].cancel()

            self._timers[key] = threading.Timer(Message.PERIOD_UPDATES, send_update)
            self._timers[key].start()

        send_update()

    def disconnect(self) -> None:
        """Disconnect from the current network."""
        # set accepting flag to false
        self._accepting = False

        to_remove = list()
        # inform all nodes
        for node in self._nodes:
            txid = self.get_txid()
            m = Message({'type': 'disconnect',
                         'txid': txid})

            # stop sending updates
            timer_key = '{}:{:d}'.format(node.ipv4, node.port)
            if self._timers[timer_key].is_alive():
                self._timers[timer_key].cancel()

            # send disconnect message
            self._locks['sock_reg'].acquire()
            self._socket_reg.sendto(m.encode(), (node.ipv4, node.port))
            self._locks['sock_reg'].release()

            # wait for the ACK for 2 secs
            self.create_timer(str(txid), Message.ACK_WAIT, 'disconnect with TXID={:d} not acknowledged'.format(txid))
            
            # stop validity timer
            node.stop_timer()

            peers_to_remove = list()
            # remove all peers registered to this address
            for peer in self._peers:
                if peer.node_ipv4 == node.ipv4 and peer.node_port == node.port:
                    peers_to_remove.append(peer)

            for peer in peers_to_remove:
                if peer in self._peers:
                    self._peers.remove(peer)
            # add to remove the node
            to_remove.append(node)

        for node in to_remove:
            self._nodes.remove(node)

    def process_disconnect(self, addr) -> None:
        """Process the DISCONNECT message."""
        # remove the node
        for node in self._nodes:
            if node.ipv4 == addr[0] and node.port == addr[1]:
                node.invalidate()
                break

    def synchronize(self) -> None:
        """Synchronize the database with other nodes."""
        # send updates to all known neighbors and restart the update timers ~ connect_and_maintain() function
        for node in self._nodes:
            self.connect_and_maintain(node.ipv4, node.port)

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
            self._accepting = True
            # update message
            m = Message({'type': 'update',
                         'txid': self.get_txid(),
                         'db': self.get_database()})

            self._locks['sock_reg'].acquire()
            self._socket_reg.sendto(m.encode(), (cmnd['reg_ipv4'], cmnd['reg_port']))
            self._locks['sock_reg'].release()

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
        # declaration
        self._socket_chat = None
        self._got_list = None

        # call UnitUDP constructor
        super().__init__(UnitUDP.SOCKET_PEER, pid, reg_ipv4, reg_port)

        # check if the chat IP address is valid
        try:
            socket.inet_aton(chat_ipv4)
            # inet_aton does accepts numbers as IP address
            if chat_ipv4.isdigit():
                raise OSError
        except OSError:
            self._error('invalid chat IPv4 address')

        self._username = username     # username
        self._chat_ipv4 = chat_ipv4   # chat IPv4 address
        self._chat_port = chat_port   # chat port
        self._peers = None            # local peer database

        # check the port
        if not (0 <= chat_port <= 65535):
            self._error('invalid port value: out of range <0,65535>')

        # socket initialization (init, bind and set to non-blocking)
        self._socket_chat = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket_chat.bind((self.chat_ipv4, self.chat_port))
        self._socket_chat.settimeout(0)

        # threads syns. mechanisms
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
        if self._socket_chat is not None:
            self.disconnect()
            self._socket_chat.close()
        
        super().__del__()
        if self._got_list is not None:
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
            self._timers['hello'] = threading.Timer(Message.PERIOD_HELLOS, send_hello)
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

        self._locks['sock_reg'].acquire()
        self._socket_reg.sendto(hello.encode(), (self.reg_ipv4, self.reg_port))
        self._locks['sock_reg'].release()

    def operate(self) -> None:
        """Start the peer functionality."""
        # connect to a node
        self.connect_and_maintain()

        while self._run:
            try:
                self._locks['sock_chat'].acquire()
                data, addr = self._socket_chat.recvfrom(UnitUDP.RECV_BUFFER)
                self._locks['sock_chat'].release()
                mess = Message(data)
                if not mess.valid:
                    self._notify('invalid message received {}'.format(str(data)), warning=True)
                    self._locks['sock_chat'].acquire()
                    self.send_error_msg(self._socket_chat, 0, 'couldn\'t decode the message: {}'.format(str(data)), addr)
                    self._locks['sock_chat'].release()
                else:
                    # message received
                    if mess['type'] == 'message':
                        # print the message
                        print('{}:> {}'.format(mess['from'], mess['message']))
                        # acknowledge the message
                        self._locks['sock_chat'].acquire()
                        self.acknowledge_msg(self._socket_chat, mess['txid'], addr)
                        self._locks['sock_chat'].release()
                    # ack received
                    elif mess['type'] == 'ack':
                        timer = self._timers.get(str(mess['txid']), None)
                        if timer is not None:
                            timer.cancel()
                    # list received
                    elif mess['type'] == 'list':
                        # acknowledge the message
                        self.acknowledge_msg(self._socket_reg, mess['txid'], addr)
                        self._peers = mess['peers']
                        self._got_list.set()  # synchronize
                    # an error received
                    elif mess['type'] == 'error':
                        self._notify('an error message received: {}'.format(mess['verbose']), warning=True)
                    # unknown message
                    else:
                        self._notify('unsupported message type received: {}'.format(mess['type']), warning=True)

            # no datagrams available
            except BlockingIOError:
                self._locks['sock_chat'].release()
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
        """Get a list of currently connected peers."""
        txid = self.get_txid()
        m = Message({'type': 'getlist',
                     'txid': txid})

        self._got_list.clear()  # sync mechanism

        self._locks['sock_chat'].acquire()
        self._socket_chat.sendto(m.encode(), (self._reg_ipv4, self._reg_port))
        self._locks['sock_chat'].release()

        # wait for the acknowledge for 2 secs
        self.create_timer(str(txid), Message.ACK_WAIT, 'getlist not acknowledged (TXID={:d})'.format(txid))
        # wait until the list is received (max 3 sec)
        self._got_list.wait(timeout=3)

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
        # get peers
        self.get_list()

        # different usernames (from RPC and peer's)
        if self.username != msg_from:
            self._notify('different peer username vs. RPC command username: \'{}\' vs. \'{}\''.format(self.username, msg_from), warning=True)

        # get the user's address
        addr = self.get_addr_by_username(msg_to)
        if addr is None:
            self._notify('no such username: \'{}\''.format(msg_to), warning=True)
            return

        # construct the message
        txid = self.get_txid()
        m = Message({'type': 'message',
                     'txid': txid,
                     'from': msg_from,
                     'to': msg_to,
                     'message': msg})

        # wait for the acknowledge for 2 secs
        self.create_timer(str(txid), Message.ACK_WAIT, 'unable to deliver the message with TXID={:d} (not acknowledged)'.format(txid))

        self._locks['sock_chat'].acquire()
        self._socket_chat.sendto(m.encode(), addr)
        self._locks['sock_chat'].release()

    def reconnect(self, ipv4, port) -> None:
        """Disconnect from the current node and connect to a new one."""
        # stop sending HELLO messages
        if self._timers['hello'].is_alive():
            self._timers['hello'].cancel()
        # disconnect from the current node
        self.disconnect()
        # clear local database
        self._peers = None
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
        self._ipv4 = str(ipv4)       # node's IPv4 address
        self._port = port            # node's port
        self._inv_func = invalidate  # invalidating function
        self._timer = None           # validity timer

    def __eq__(self, other) -> bool:
        """NodeRecord equality function."""
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
        self._timer = threading.Timer(Message.TIMEOUT_UPDATE, self._inv_func, args=[self])
        self._timer.start()

    def stop_timer(self) -> None:
        """Stop the validity timer."""
        if self._timer is not None and self._timer.is_alive():
            self._timer.cancel()

    def invalidate(self) -> None:
        """Invalidate the NodeRecord."""
        self.stop_timer()
        self._inv_func(self)

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
        self._username = username     # peer's username
        self._ipv4 = str(ipv4)        # peer's chat IPv4 address
        self._port = port             # peer's chat port
        self._node_ipv4 = node_ipv4   # peer's IPv4 address of its registration node
        self._node_port = node_port   # peer's port of its registration node
        self._inv_func = invalidate   # invaliding function when the timer runs out
        self._timer = None            # validity timer

    def __eq__(self, other) -> bool:
        """PeerRecord equality function."""
        return self.username == other.username and \
               self.ipv4 == other.ipv4 and \
               self.port == other.port

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
        """Update the peer record and restart its timer."""
        self._username = peer.username
        self._ipv4 = peer.ipv4
        self._port = peer.port
        self._node_ipv4 = peer.node_ipv4
        self._node_port = peer.node_port

        # restart timer
        self.start_timer()

    def start_timer(self) -> None:
        """Restart the validity timer."""
        # validity timer (30 seconds)
        self.stop_timer()
        self._timer = threading.Timer(Message.TIMEOUT_HELLO, self._inv_func, args=[self])
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
        try:
            self._socket_rpc.connect(self._socket_rpc_file)
        except Exception:
            print('Error: unable to connect to the specified {}'.format(self.dest), file=sys.stderr)
            sys.exit(1)

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

        self._socket_rpc.send(cmd.encode())  # TODO: sendto(cmd.encode(), self._socket_rpc_file)
