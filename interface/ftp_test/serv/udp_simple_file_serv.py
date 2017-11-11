import socket
import time
import struct

UDP_IP = "127.0.0.1"
UDP_PORT_TX = 5006
UDP_PORT_RX = 5007

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT_RX))

def chunks(l, n):
  chunks = []
  pos = 0
  while pos < len(l):
    length = n
    if(pos + n > len(l)):
      length = len(l) - (pos + n)
    chunks.append(l[pos:pos+length])
    pos = pos + n
  return chunks

while True:
  data, addr = sock.recvfrom(2048)
  blocks = None
  fn_len = struct.unpack('<i', data[:4])[0]
  filename = data[4:4+fn_len]
  if len(data) > 4+fn_len:
    blocks = data[4+fn_len:]
    blocks = struct.unpack('<%ui' % (len(blocks)/4), blocks)
    print blocks

  print filename
  time.sleep(0.5)

  with open(filename, 'r') as f:
    dat = chunks(f.read(), 128)
    count = 0
    for chunk in dat:
      if blocks is None or count in blocks:
        print count
        chunk = struct.pack("<i", count) + struct.pack("<i", len(dat)) + chunk
        sock.sendto(chunk, (UDP_IP, UDP_PORT_TX))
        time.sleep(0.001)
      count = count + 1




