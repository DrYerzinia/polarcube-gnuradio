import socket
import time

UDP_IP = "127.0.0.1"
UDP_PORT_TX = 5006
UDP_PORT_RX = 5007

FILENAME = "mwtbdltr.jpg"

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT_RX))

last_block = -1
error_count = 0

with open(FILENAME, 'w') as f:
  while True:
    data, addr = sock.recvfrom(2048)
    cur_block = int(data[:4])

    if(last_block + 1 != cur_block):
        error_count = error_count + 1
        print "Error missing blocks"
        print error_count

    print "Block: " + str(cur_block)

    f.write(data[4:])
    if(len(data) != 128+4):
      break
    last_block = cur_block
