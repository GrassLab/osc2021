#!python3
import sys
import os
import serial

ser = None
#kernel = open('./kernel8.img', 'rb')

if len(sys.argv) == 2:
    ser = serial.Serial(sys.argv[1], timeout=0.1)
elif len(sys.argv) == 3:
    ser = serial.Serial(sys.argv[1], int(sys.argv[2]), timeout=0.1)

def nonblock_read ():
    while True:
        b = ser.read(1)
        if not b:
            break
        try:
            print(b.decode('utf-8'), end='')
            sys.stdout.flush()
        except:
            print(b)

def nonblock_clear ():
    while True:
        b = ser.read(1)
        if not b:
            break

def get_line ():
    buf = b''
    while True:
        b = ser.read(1)
        if b == b'\n':
            break
        buf += b
    return buf.decode('utf-8')

def send (message):
    message = str.encode(message + '\n')
    ser.write(message)

def load_kernel (fileName):
    size = os.path.getsize(fileName)
    base_address = 0x80000
    aligned_size = (size // 8) * 8 + 16
    nonblock_read()
    send('load')
    send(f'{base_address}')
    nonblock_clear()
    send(f'{aligned_size}')

    f = open(fileName, 'rb')
    chunk = []
    longb = b''
    checksum = 0
    while True:
        c = f.read(1)
        if not c:
            break
        longb += c
        if len(longb) == 8:
            ser.write(longb)
            checksum ^= int.from_bytes(longb, byteorder='little')
            longb = b''

    longb += b'\x00' * (8 - size % 8)
    end_tag = 0x00000000deadbeef
    checksum ^= int.from_bytes(longb, byteorder='little') ^ end_tag
    longb += end_tag.to_bytes(8, byteorder='little')
    ser.write(longb)
    for i in range(5):
        print(get_line())
    checksum_str = get_line()
    checksum_recv = int(checksum_str.split(' ')[1], 0)
    print(checksum_str)
    if checksum_recv == checksum:
        print('\nsuceed to load kernel~~~~~~')
        nonblock_read()
        return True
    else:
        print('\n!!!! fail to load kernel !!!!')
        nonblock_read()
        return False

while True:
    command = input()
    if command == 'exit':
        break
    if command == 'load':
        load_kernel ('kernel8.img')
        continue

    if command.find('LOAD ') == 0:
        load_kernel (command.split(" ")[1])
        continue

    command = str.encode(command + '\n')
    ser.write(command)
    nonblock_read()

ser.close()
