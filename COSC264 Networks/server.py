# socket server - TCP. Allows exchange of control and file data. Server
# application accepts one paramter, which is the port number, between 1024 and 64000
import socket
import sys
import datetime
import time

port = int(sys.argv[1])
host = ''                 # Symbolic name meaning all available interfaces

# Do checks on command line inputs
if port < 1024 or port > 64000:
    sys.exit('Port number should be between 1024 and 64000. Port was: {}'.format(port))

# create socket
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error as e:
    sys.exit('Error creating socket, ERROR: {}'.format(e))

# bind socket to port
try:
    s.bind((host, port))
except socket.error as e:
    s.close()
    sys.exit('Error binding socket to port {}, ERROR: {}'.format(port, e))

# listen, with buffer size 1
try:
    s.listen(1)
    print('Socket listening at host {} on port {}'.format(socket.gethostname(), port))
except socket.error as e:
    s.close()
    sys.exit('Error listening from socket, ERROR: {}'.format(e))

# accept connection request
while True:
    conn, addr = s.accept()
    conn.settimeout(1)
    print('Connection accepted at {} from socket with IP {}, port {}'.format(datetime.datetime.now(), addr[0], addr[1]))

    # read filerequest header
    try:
        buff = b''
        buff += conn.recv(5)
        while len(buff) < 5:
            buff += conn.recv(5-len(buff))
    except socket.timeout:
        print('FileRequest header was not received fully before timeout')
        conn.close()
        continue

    # check fileRequest header
    magicno = int.from_bytes(buff[0:2], byteorder='big', signed=False)
    s_type = buff[2]
    n = int.from_bytes(buff[3:5], byteorder='big', signed=False)
    if magicno != 0x497E:
        conn.close()
        print('Magic number was incorrect, found {}'.format(magicno))
        continue
    elif s_type != 1:
        conn.close()
        print('Type was incorrect, found {}'.format(s_type))
        continue
    elif n < 1 or n > 1024:
        conn.close()
        print('Filename length was wrong size, found {}'.format(n))
        continue
    else:
        # read file name
        try:
            buff = b''
            buff += conn.recv(n)
            while len(buff) < n:
                buff += conn.recv(n-len(buff))
        except socket.timeout:
            print('FileRequest filename was not received fully before timeout')
            conn.close()
            continue
        # check if filename is longer than n
        try:
            if len(conn.recv(1)) > 0:
                print('Total bytes received was more than the filenamesize from the Request header ({})'.format(n))
                conn.close()
                continue
        except:
            pass

        # open the file for reading and prepare fileResponse
        filename = buff.decode('utf-8')
        magicno = (0x497E).to_bytes(2, byteorder='big', signed=False)
        s_type = b'\2'
        dataLen = bytes(4)
        try:
            f = open(filename,'rb')
            data = f.read()
            n = len(data)

            # write fileResponse message
            statusCode = b'\1'
            dataLen = (n).to_bytes(4, byteorder='big', signed=False)

            response = magicno + s_type + statusCode + dataLen + data
            f.close()
        except:
            # write fileResponse error message
            statusCode = b'\0'
            response = magicno + s_type + statusCode + dataLen

        # send fileResponse
        # Use sendall rather than send to guarentee all data is sent.
        conn.sendall(response)
        if statusCode == b'\0':
            print('Could not open file {}. FileResponse sent'.format(filename))
        else:
            print('Finished sending file {}, total {} bytes'.format(filename, n))
        conn.close()
