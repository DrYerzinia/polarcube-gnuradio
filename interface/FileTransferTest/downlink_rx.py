#!/usr/bin/python3

import hashlib, binascii
import base64
import os
import sys
import socket
import time
import struct
from subprocess import call,Popen
from bitarray import bitarray

CHUNK_SIZE = 128

rx_port = 35777

#Setting up the socket
sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
try:
    sock.bind(("0.0.0.0", rx_port))
except socket.error as e:
    print(e, flush=True)

sock.settimeout(3)

while True:

    print("Waiting for file info", flush=True)
    try:
      fileinfo, addr = sock.recvfrom(2048)
    except socket.timeout:
      continue

    msg_type = fileinfo[0]
    if msg_type == 1:
      print(fileinfo, flush=True)

      blocks = struct.unpack("<H", fileinfo[2:4])[0]
      filename = "rx_" + struct.unpack("32s", fileinfo[4:])[0].split(b'\0', 1)[0].decode("utf-8")

      r_blocks = blocks * bitarray('0')

      print("FileInfo received %s %d" % (filename, blocks), flush=True)

      missing_retry_counter = 0

      with open(filename, "wb") as f:

        while True:
          try:

            fileblock, addr = sock.recvfrom(2048)

            msg_type = fileblock[0]
            if msg_type == 2:

                try:

                    blen = fileblock[1]
                    bnum = struct.unpack("<H", fileblock[2:4])[0]

                    data = struct.unpack("%us" % blen, fileblock[4:4+blen])[0]

                    print("Got block %d" % bnum, flush=True)

                    r_blocks[bnum] = True;

                    f.seek(CHUNK_SIZE*bnum)
                    f.write(data)
                except:
                    pass

          except socket.timeout:
            missing = []
            for i in range(len(r_blocks)):
              if not r_blocks[i]:
                missing.append(i)

            if len(missing) == 0:
              print("File RXed", flush=True)
              break
