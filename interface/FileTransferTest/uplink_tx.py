#!/usr/bin/python3

import hashlib, binascii
import base64
from subprocess import call
import subprocess
import os
import sys
import socket
import time
import struct
from collections import OrderedDict
from pprint import pprint
import random
from serial import Serial

CHUNK_SIZE = 128

def chunks(l, n):
  chunks = []
  pos = 0
  while pos < len(l):
    length = n
    if(pos + n > len(l)):
      length = len(l) - (pos + n)
    if length < 0:
        length = length + n
    chunks.append(l[pos:pos+length])
    pos = pos + n
  return chunks

def frame(data):
    data = data.replace(b"\xDB", b"\xDB\xDD")
    data = data.replace(b"\xC0", b"\xDB\xDC")
    data = b"\xC0" + data + b"\xC0"
    return data

#def deframe(data):
#    data.replace(b"\xDB\xDC", b"\xC0")
#    data.replace(b"\xDB\xDD", b"\xDB")

tty = sys.argv[1]
filename = sys.argv[2]

ser = Serial(port=tty, baudrate=115200)

fileinfo = []

#Getting number of blocks in a file
with open(filename, 'rb') as f:
    chunked_file = chunks(f.read(), CHUNK_SIZE)

fileinfo.append(struct.pack("BH32s", int(1), len(chunked_file), filename.split('/')[-1].encode("utf-8"))) # type Pack

#Getting file size
statinfo = os.stat(filename)

fileinfo_packed = b"".join(fileinfo)

for i in range(2):
  print("Sending Fileinfo of", filename+"...", flush=True)
  ser.write(frame(fileinfo_packed))

  time.sleep(1.5)

chunked_file_tmp = chunked_file
for idx, chunk in enumerate(chunked_file_tmp):
    chunked_file_tmp[idx] = (struct.pack("<"+str(len(chunk))+"B", *chunk), idx)

print("Sending blocks:", [x[1] for x in chunked_file_tmp], flush=True)
for ch_tuple in chunked_file_tmp:
    chunk_num = ch_tuple[1]
    chunk = ch_tuple[0]
    chunk = struct.pack("<B", int(2)) + struct.pack("<B", int(len(chunk))) + struct.pack("<H", int(chunk_num)) + chunk
    ser.write(frame(chunk))
    time.sleep(0.1)

print("Finished sending file " + filename, flush=True)
