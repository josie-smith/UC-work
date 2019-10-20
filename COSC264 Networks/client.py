# socket client - TCP. Downloads a file of its choosing from the server
# accepts 3 paramters, IP/ hostname, port and filename
import socket
import sys
import os.path
import time

host = sys.argv[1]
port = int(sys.argv[2])
filename = sys.argv[3]

# Do checks on command line inputs
try:
    ip = socket.gethostbyname(host)
except socket.error as e:
    sys.exit('Error converting hostname {}, ERROR: {}'.format(host, e))
if port < 1024 or port > 64000:
    sys.exit('Port number should be between 1024 and 64000. Port was: {}'.format(port))

# check if file is already available locally
if os.path.isfile(filename):
    sys.exit('File {} already exists locally'.format(filename))

# check if file can be opened
canbeopened = True
try:
    with open(filename) as f:
        s = f.read()
except IOError as e:
    canbeopened = False
if canbeopened:
    sys.exit('File {} already exists locally and can be opened'.format(filename))

# check all inputs are there
if len(sys.argv) != 4:
    sys.exit('Wrong number of input paramters, expecting host, port and filename')

# create socket
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(1)
except socket.error as e:
    sys.exit('Error creating socket on host {}, ERROR: {}'.format(ip, e))
# connect socket to port
try:
    s.connect((ip, port))
except socket.error as e:
    s.close()
    sys.exit('Error connecting to {}, port {}, ERROR: {}'.format(ip, port, e))


# prepare FileRequest
magicno = (0x497E).to_bytes(2, byteorder='big', signed=False)
s_type = b'\1'
filename_b = filename.encode('utf-8')
n = len(filename_b)
filenameLen = (n).to_bytes(2, byteorder='big', signed=False)
if n < 1 or n > 1024:
    sys.exit('File name length was wrong size, found {}'.format(n))
request = magicno + s_type + filenameLen + filename_b

# send fileRequest
s.sendall(request)

# read FileResponse header
try:
    buff = b''
    buff += s.recv(8)
    while len(buff) < 8:
        buff += s.recv(8-len(buff))
except socket.timeout:
    s.close()
    sys.exit('FileResponse was erroneous - not received fully before timeout')

magicno = int.from_bytes(buff[0:2], byteorder='big', signed=False)
s_type = buff[2]
statusCode = buff[3]
n = int.from_bytes(buff[4:8], byteorder='big', signed=False)

# check fileResponse header
if magicno != 0x497E:
    s.close()
    sys.exit('Magic number was incorrect, found {}'.format(magicno))
elif s_type != 2:
    s.close()
    sys.exit('Type was incorrect, found {}'.format(s_type))
elif statusCode != 0 and statusCode != 1:
    s.close()
    sys.exit('StatusCode was not 0 or 1, found {}'.format(statusCode))
elif statusCode == 0:
    s.close()
    sys.exit('Server could not send the file')
elif statusCode == 1:
    # open file locally for writing
    try:
        f = open(filename, 'wb+')
    except:
        s.close()
        sys.exit('File could not be opened locally for writing')

    try:
        # read file data
        buff = b''
        totalbytes = 0
        while True:
            data = s.recv(4096)
            buff += data
            f.write(buff)
            totalbytes += len(data)
            if len(data) < 4096:
                # end of file
                break

    except socket.timeout:
        f.close()
        s.close()
        sys.exit('FileResponse was erroneous - not received fully before timeout')

    # Check data read is the length the fileResponse said
    if totalbytes != n:
        f.close()
        s.close()
        sys.exit('Total bytes received ({}) was not the same as filesize from the Response header ({})'.format(totalbytes, n))

    print('Finished writing data to file, received {} bytes total'.format(totalbytes))

    f.close()
    s.close()
