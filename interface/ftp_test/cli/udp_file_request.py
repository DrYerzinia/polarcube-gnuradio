import socket
import time
import struct

UDP_IP = "127.0.0.1"
UDP_PORT_TX = 5006
UDP_PORT_RX = 5007

#FILENAME = "a.jpg"
#FILENAME = "mwtbdltr.jpg"
FILENAME = "great_expectations_transmit.txt"
#FILENAME = "pulp_fiction_transmit.txt"

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT_RX))

msg = struct.pack("<i", len(FILENAME)) + FILENAME
sock.settimeout(5)
sock.sendto(msg, (UDP_IP, UDP_PORT_TX))

last_block = -1

blocks = {}
missing_blocks = []
num_blocks = -1

first_run = True

while True:
  while True:
    try:
     data, addr = sock.recvfrom(2048)
     cur_block = struct.unpack("<i", data[:4])[0]
     num_block = struct.unpack("<i", data[4:8])[0]
     if num_blocks == -1:
       print "Num Blocks:", num_blocks
       num_blocks = num_block
       missing_blocks = range(0,num_blocks)

     print "Block: " + str(cur_block)

     blocks[cur_block] = data[8:]
     try:
       missing_blocks.remove(cur_block)
     except:
       pass

    except socket.timeout:
     print "timeout"
     if first_run:
       sock.sendto(msg, (UDP_IP, UDP_PORT_TX))
       continue
     break

    first_run = False

  print missing_blocks
  if len(missing_blocks) > 0:
    mblock = missing_blocks[:]
    if len(mblock) > 30:
      mblock = mblock[:30]
    msg = struct.pack("<i", len(FILENAME)) + FILENAME + struct.pack("<%ui" % len(mblock), *mblock)
    sock.sendto(msg, (UDP_IP, UDP_PORT_TX))

  if not missing_blocks:
    break

#print blocks
with open(FILENAME, 'w') as f:
  for key, value in blocks.iteritems():
    f.write(value)




