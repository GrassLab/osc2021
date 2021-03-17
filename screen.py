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
    # remove \r
    return buf.decode('utf-8')[:-1]

def get_until (token):
    buf = b''
    while True:
        buf += ser.read(1)
        if buf.find(str.encode(token)) >= 0:
            break
    return buf.decode('utf-8')


def send (message):
    message = str.encode(message + '\n')
    ser.write(message)

def load_data (buf, base_address=0x80000):
    base_address = 0x80000
    buf += (8 - (len(buf) % 8)) * b'\x00'
    end_tag = 0x00000000deadbeef
    buf += end_tag.to_bytes(8, byteorder='little')
    aligned_size = len(buf)

    send('load')
    nonblock_clear()
    send(f'{base_address}')
    nonblock_clear()
    send(f'{aligned_size}')

    while True:
        line = get_line()
        # wait for 0x0
        if line == '0x0':
            break
        else:
            print(line)

    print("start sending:")
    checksum = 0x0
    count = 0
    totalCount = 40
    isFail = False
    print("[", end='')
    sys.stdout.flush()

    for i in range(aligned_size // 8):
        chunck = buf[i * 8: i * 8 + 8]
        ser.write(chunck)
        checksum ^= int.from_bytes(chunck, byteorder='little')
        recv_check = int(get_line(), 0)
        if checksum == recv_check:
            pass
        else:
            isFail = True
        currCount = i * 8 * totalCount // aligned_size
        print("=" * (currCount - count), end='')
        sys.stdout.flush()
        count = currCount

    if not isFail:
        print("=" * (totalCount - count) + "] 100%")


def load_kernel (fileName):
    size = os.path.getsize(fileName)
    f = open(fileName, 'rb')
    image = b''
    while True:
        c = f.read(1)
        if not c:
            break
        image += c
    load_data(image)

def auto_load ():
    load_kernel('kernel8.img')
    get_until('$ ')
    send('jump')
    print(get_until('$ '), end='')


#auto_load()

while True:
    command = input()
    if command == 'exit':
        break

    if command == 'load':
        auto_load()
        continue

    if command == 'reboot':
        send('reboot')
        print(get_until('bootloader'), end='')
        print(get_until('$ '), end='')
        continue

    if command.find('LOAD ') == 0:
        load_kernel (command.split(" ")[1])
        continue

    command = str.encode(command + '\n')
    ser.write(command)
    nonblock_read()

ser.close()
