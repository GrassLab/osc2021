
'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''

import sys
sys.path
sys.path.append('/home/jinbao/.local/lib/python3.8/site-packages')
import os
import serial
from time import sleep

device = '/dev/ttyUSB0'
# device = '/dev/pts/2'
ser = serial.Serial(device, 115200)    #Open port with baud rate

tty = open(device, 'wb', buffering = 0)
f = open('kernel8.img', 'rb')

'''=========================================='''
data_left = ser.inWaiting()
if data_left:
    print(ser.read(data_left))
print('========== nothing left now ===========')
tty.write(b'load\r')
sleep(1)
data_left = ser.inWaiting()
while data_left:
    recv_data = ser.read(data_left)
    print(recv_data)
    data_left = ser.inWaiting()
if b'size of kernel8.img' not in recv_data:
    print('Error: size of kernel8.img')
kernel_size = str(os.stat('kernel8.img').st_size) + '\r'
# tty.write(b'1536\r')
tty.write(kernel_size.encode())
sleep(1)
data_left = ser.inWaiting()
while data_left:
    recv_data = ser.read(data_left)
    print(recv_data)
    data_left = ser.inWaiting()
if b'start sending' not in recv_data:
    print('Error: sending data')

byte = f.read(1)
checksum = 0
checksum2 = 0
while byte:
    tty.write(byte)
    byte = f.read(1)
    checksum2 += 1
    if byte:
        checksum += 1
        # checksum += byte[0]

print('Client checksum: ' + str(checksum))
print('Client checksum2: ' + str(checksum2))

print('Sending completed!')
sleep(1)
data_left = ser.inWaiting()
while data_left:
    recv_data = ser.read(data_left)
    print(recv_data)
    data_left = ser.inWaiting()