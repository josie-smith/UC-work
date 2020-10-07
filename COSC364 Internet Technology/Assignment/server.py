# COSC364 Assignment 1 - RIPv2 protocol
# Authors:
# Josie Smith - 82466864
# Huba Nagy - 86064755

import socket
import sys
import os
import datetime
import time
import select
import struct
import time
import random
import logging
import signal

logging.basicConfig(level=os.getenv('LOG_LEVEL', 'INFO'))
log = logging.getLogger('ripd')

running = True
def exit_handler(sig, frame):
    global running
    running = False
signal.signal(signal.SIGINT, exit_handler)
signal.signal(signal.SIGTERM, exit_handler)

local_host = '127.0.0.1'
host = ''                   # Symbolic name meaning all available interfaces
period = 30
my_id = 0
input_ports = []
output_ports = []
sockets = []
forwarding = {}

class ParseException(Exception):
    pass

def assert_number_between(field_name, raw, minimum, maximum):
    if raw.isdigit() and int(raw) <= maximum and int(raw) >= minimum:
        return int(raw)
    else:
        raise ParseException('Expected {} to be an integer between {} and {}.'.format(field_name, minimum, maximum))

def assert_array_length(field_name, array, length):
    if len(array) == length:
        return True
    else:
        raise ParseException('Expected {} value(s) in {}.'.format (length, field_name))

def assert_not_in(field_name, raw, array_name, array):
    if raw not in array:
        return True
    else:
        raise ParseException('Expected {} to not be in the list of {}.'.format (field_name, array_name))

def assert_in(field_name, raw, array_name, array):
    if raw in array:
        return True
    else:
        raise ParseException('Expected {} to be in the list of {}.'.format (field_name, array_name))

def loadConfig(configfile):
    # We will throw parse error if this set is not empty at the end of
    # the config file
    required = set(['router-id', 'neighbour'])
    config = {'timer': 30, 'input-ports': [], 'output-ports': []}

    def parse_line(line):
        nonlocal required
        nonlocal config
        words = line.split('#')[0].split()
        if len(words) == 0:
            return

        if words[0] == 'router-id':
            assert_array_length('router-id', words[1:], 1)
            config['router-id'] = assert_number_between('router-id', words[1], 1, 64000)
            required.discard('router-id')

        elif words[0] == 'neighbour':
            assert_array_length('neighbour', words[1:], 4)

            output = {}
            output['peer_id'] = assert_number_between('neighbour.peer-id', words[1], 1, 64000)
            output['port'] = assert_number_between('neighbour.port', words[2], 1024, 64000)
            output['metric'] = assert_number_between('neighbour.metric', words[4], 1, 16)

            input_port = assert_number_between('neighbour.input-port', words[3], 1024, 64000)
            assert_not_in('neighbour.input-port', input_port, 'input-ports', config['input-ports'])
            config['input-ports'].append(input_port)
            assert_not_in('neighbour.output-port', output['port'], 'input-ports', config['input-ports'])
            assert_not_in('neighbour.output-port', output['port'], 'output-ports', config['output-ports'])
            config['output-ports'].append(output)

            required.discard('neighbour')

        elif words[0] == 'timer':
            assert_array_length('timer', words[1:], 1)
            config['timer'] = assert_number_between('timer', words[1], 1, 1800)
            # timer not required

        else:
            raise ParseException("Unknown config command {}".format(words[0]))

    with open(configfile) as f:
        line_count = 1
        for line in f:
            try:
                parse_line(line)
                line_count += 1
            except ParseException as e:
                raise ParseException("{f}: {e}\n\tline {n}: {l}".format(f=configfile, n=line_count, e=e, l=line))

    if len(required) != 0:
        raise ParseException("Missing required fields: {}".format(', '.join(required)))

    return config


def initSockets():
    global sockets
    # Create UDP sockets for each input port
    for i in range (len(input_ports)):
        # create socket
        sockets.append(socket.socket(socket.AF_INET, socket.SOCK_DGRAM))

        # bind socket to port
        sockets[i].bind((host, input_ports[i]))
        log.info("Listening on port {}".format(input_ports[i]))

def closeSockets():
    global sockets
    log.info('Closing sockets...')
    for sock in sockets:
        sock.close()


def regUpdate():
    triggered = False
    # output process - always send to all neighbours as we are not processing request messages
    # destination address is each output_ports
    for neighbour in output_ports:
        try:
            # RIP packet format: command (response, 1 byte), version (value is 2, 1 byte), sender address (2 bytes), RIP entries (between 1 and 25)
            command = struct.pack(">B", 2)
            version = struct.pack(">B", 2)
            sender = struct.pack(">H", my_id)
            header = command + version + sender

            print_rte = []
            RTEs = []
            for destination in forwarding:
                afi = struct.pack('>H', 2)
                dest = struct.pack('>I', destination)
                # If the route has X as the next hop, then set the metric of that route to 16 in messages to X.
                if forwarding[destination]['next_hop'] == neighbour['peer_id']:
                    metric = struct.pack('>I', 16)
                else :
                    metric = struct.pack('>I', forwarding[destination]['metric'])

                RTEs.append(afi + struct.pack('>H', 0) + dest + struct.pack('>Q', 0) + metric)
                print_rte.append(struct.unpack('>II', dest + metric))

            if len(RTEs) == 0:
                sockets[0].sendto(header, (local_host, neighbour['port']))

            # limit to 25 RTEs per message
            while len(RTEs) > 0:
                count = 0
                rte = b""
                while count < len(RTEs) and count < 25:
                    rte += RTEs[count]
                    count += 1
                sockets[0].sendto(header + rte, (local_host, neighbour['port']))
                del RTEs[:25]
                log.debug( "sent message to {}: {}".format(neighbour['peer_id'], print_rte))

        except socket.error as e:
            log.error('Error writing to socket {}, ERROR: {}'.format(neighbour['port'], e))


def recvPacket(sock, now):
    # receive a routing packet, it will be response as we are not implementing request messages
    try:
        data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    except socket.error:
        log.error('Tried to read from a socket where no data is available.')
        return

    # check it is from a valid port and the packet is not from this router
    if (addr[1] in input_ports) or (addr[1] < 1) or (addr[1] > 64000):
        return

    # use the 16 bits of zeroes in the RIP common header for the router-id
    (command, version, router_id) = struct.unpack('>BBH', data[:4])
    valid_header = True
    if command != 2: # command = 2 if the message is a response rather than request
        valid_header = False
        log.error('Packet header error: command was not 2, received {}'.format(command))
    elif version != 2: # RIP version should be 2
        valid_header = False
        log.error('Packet header error: version was not 2, received {}'.format(version))
    elif router_id not in [x['peer_id'] for x in output_ports]:
        # This check already implies that that router_id is a valid ID and not our own
        valid_header = False
        log.error('Packet header error: router_id was not valid, received {}'.format(router_id))

    if not valid_header:
        return


    # add the originating router to the forwarding table even if no RTEs are sent
    metric_to_next_hop = [x['metric'] for x in output_ports if x['peer_id'] == router_id][0]
    update_record(router_id, router_id, metric_to_next_hop, now)


    rip_entry = data[4:]
    rtes_read = 0
    while len(rip_entry) > 0 and rtes_read < 20:
        (afi, z0, destination, z1, metric) = struct.unpack('>HHIQI', rip_entry[:20])
        log.debug("Received ({}, {}) from {}".format( destination, metric, router_id))
        rip_entry = rip_entry[20:]
        rtes_read += 1

        valid_rte = True
        if afi != 2: # AFI version should be 2 for IP
            valid_rte = False
            log.error('Read error in rte {}: afi was not 2, received {}'.format(rtes_read, afi))
        elif z0 != 0 or z1 != 0:
            valid_rte = False
            log.error('Read error in rte {}: zeroes were not zero, received {} {}'.format(rtes_read, z0, z1))
        elif destination < 1 or  destination > 64000:
            valid_rte = False
            log.error('Read error in rte {}: destination was invalid, received {}'.format(rtes_read, destination))
        elif destination == my_id:
            valid_rte = False
            # This is not technically an error.
            log.debug('Received rte about distance to myself, disregarding.')
        elif metric < 1 or metric > 16:
            valid_rte = False
            log.error('Read error in rte {}: metric was invalid, received {}'.format(rtes_read, metric))

        if valid_rte:
            # Add the next hop's distance to the metric, but cap at 16.
            metric = min(metric_to_next_hop + metric, 16)
            update_record(destination, router_id, metric, now)


def update_record(destination, next_hop, metric, now=None):
    if now == None:
        now = time.time()

    forwarding_entry = {
            'metric': metric,
            'next_hop': next_hop,
            'timer': now + 6 * period,
            'garbage_timer': float('inf')}

    updated = False
    if destination in forwarding:
        old_metric = forwarding[destination]['metric']
        old_next_hop = forwarding[destination]['next_hop']
        old_timer = forwarding[destination]['timer']

        if next_hop == old_next_hop:
            if metric > old_metric and metric == 16:
                schedule_deletion(destination, now)

            elif metric != old_metric:
                # if the metric is different to before, we need to refresh the metric and next hop
                forwarding[destination] = forwarding_entry
                updated = True

            elif metric == old_metric and metric != 16 and old_timer < (now + 3 * period):
                forwarding[destination] = forwarding_entry
                updated = True

        elif metric < old_metric:
            forwarding[destination] = forwarding_entry
            updated = True

    elif metric != 16:
        forwarding[destination] = forwarding_entry
        updated = True

    if updated:
        log.debug("Updated destination {}: {}".format(destination, forwarding_entry))


def checkExpired(now):
    remove = []
    for destination in forwarding:
        if forwarding[destination]['timer'] < now:
            if forwarding[destination]['garbage_timer'] < now:
                remove.append(destination)

            elif forwarding[destination]['garbage_timer'] == float('inf'):
                schedule_deletion(destination, now)

    for destination in remove:
        log.debug("Deleting {} at {}".format(destination, now))
        del forwarding[destination]


def schedule_deletion(destination, now):
    log.debug("Scheduling deletion of {} at {}".format(destination, now))
    forwarding[destination]['garbage_timer'] = now + 4*period
    forwarding[destination]['metric'] = 16
    triggered = True



try:
    log.info("Reading configuration from {}...".format(sys.argv[1]))
    config = loadConfig(sys.argv[1])
    my_id = config['router-id']
    log = logging.getLogger("ripd-{}".format(my_id))
    input_ports = config['input-ports']
    output_ports = config['output-ports']
    period = config['timer']
except FileNotFoundError:
    log.critical("Config error: can't find {}".format(sys.argv[1]))
    sys.exit(1)
except ParseException as e:
    log.critical("Config error: {}".format(e))
    sys.exit(1)

try:
    log.info("Binding sockets...")
    initSockets()
except socket.error as e:
    log.critical("Socket error: {}".format(e))
    closeSockets()
    sys.exit(2)

log.info("Starting up router...")
start_time = time.time()
next_update = start_time
current_time = start_time
trigger_buffer = 0
triggered = False

# react to incoming events
while running:
    timeout = (next_update - current_time)
    readable, writable, exceptional = select.select(sockets, [], sockets, timeout)
    if len(exceptional) > 0:
        console.log('Some sockets failed.')
        sockets.close()
        sys.exit(3)

    current_time = time.time()
    for sock in readable:
        recvPacket(sock, current_time)

    checkExpired(current_time)

    if current_time >= next_update:
        # introduce randomness by setting timer to a value that is uniformly distributed over the range [0.8*period,1.2*period]
        next_update = current_time + random.uniform(0.8, 1.2)*period
        regUpdate()

        log.info("Forwarding table at t+{:0.4f}s".format(current_time-start_time))
        for destination, entry in forwarding.items():
            print('{}: {}'.format(destination, entry))
        print ('') # empty line to signal the end of the forwarding table

    elif triggered and current_time >= trigger_buffer:
        # throttle sporadic updates to at most 30 times per period
        trigger_buffer = current_time + random.uniform(period/30, period/6)
        regUpdate()

log.info('Stopping router...')
closeSockets()
