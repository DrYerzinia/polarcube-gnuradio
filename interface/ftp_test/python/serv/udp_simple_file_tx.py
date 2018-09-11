import socket
import time

UDP_IP = "127.0.0.1"
UDP_PORT = 5006

FILENAME = "mwtbdltr.jpg"
#FILENAME = "pulp_fiction_transmit.txt"
#FILENAME = "great_expectations_transmit.txt"

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

def chunks(l, n):
    # For item i in a range that is a length of l,
    for i in range(0, len(l), n):
        # Create an index range for l of n items:
        yield l[i:i+n]

with open(FILENAME, 'r') as f:
    dat = chunks(f.read(), 128)
    count = 0
    for chunk in dat:
      chunk = "%04d" % count + chunk
      sock.sendto(chunk, (UDP_IP, UDP_PORT))
      time.sleep(0.001)
      count = count + 1
