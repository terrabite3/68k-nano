#!/usr/bin/env python3

# Very basic serial loader.
# Does no checking/verification that the upload succeeded
# Code is always loaded into the same location in RAM. (APPMEMSTART)
#
# protocol is:
# - host (this script) sends break condition
# - target (the board) replies with ASCII U
# - host sends entire binary image
# - host sends break condition
# - target reboots

import serial
import sys
import time
import glob

BAUD = 57600

if len(sys.argv) != 2:
    print('usage: serload.py <binfile>')
    sys.exit(1)

serial_devs = glob.glob('/dev/cu.usbserial-*')
if len(serial_devs) == 0:
    print("Couldn't find a serial port")
    sys.exit(1)

serial_dev = serial_devs[0]

with open(sys.argv[1], 'rb') as infile:
    ser = serial.Serial(serial_dev, BAUD, timeout=1)
    # enter loader
    ser.send_break()
    # wait for acknowledge
    ack = ser.read(5)
    print(ack)
    if ack != b'U':
      raise IOError('device did not acknowledge')
    # write bytes
    ser.write(infile.read())
    ser.flush()
    # additional delay sometimes required when sending short files
    # on macOS (dee to closing the serial port too soon?
    time.sleep(1)
    # send a break to finish
    ser.send_break()

