#!python3
import sys
import os
import serial
import time
import argparse
import threading
import signal

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
    try:
        return buf.decode('utf-8')
    except:
        print(buf)
        return "okay"

def send (message):
    message = str.encode(message + '\n')
    ser.write(message)

def load_data (buf, base_address=0x80000):
    base_address = 0x80000
    buf += (8 - (len(buf) % 8)) * b'\x00'
    end_tag = 0x00000000deadbeef
    buf += end_tag.to_bytes(8, byteorder='little')
    aligned_size = len(buf)

    is_load = False
    for i in range(5):
        time.sleep(0.1)
        nonblock_clear()
        send('load')
        if get_line() == 'okay':
            is_load = True
            break
    if not is_load:
        return False

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
        return True
    return False


def load_kernel (fileName):
    size = os.path.getsize(fileName)
    f = open(fileName, 'rb')
    image = b''
    while True:
        c = f.read(1)
        if not c:
            break
        image += c
    return load_data(image)

def auto_load ():
    if not load_kernel('kernel8.img'):
        print("load failed")
        return

    get_until('$ ')
    send('jump')
    print(get_until('$ '), end='')

is_end = False
def read_thread ():
    while not is_end:
        nonblock_read()

def sig_handler (sig, frame):
    if sig == signal.SIGINT:
        is_end = True
        os._exit(0)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('fd', type=str, help="fd's path")
    parser.add_argument('-rate', type=int, default=115200, help='baud rate')
    parser.add_argument('-load', action='store_true', help='load kernel rate automatically')
    args = parser.parse_args()
    ser = None

    if args.fd.find('ttyUSB') > 0:
        try:
            ser = serial.Serial(args.fd, args.rate, timeout=0.1)
        except:
            print(f'Error: {args.fd} not found.')
            sys.exit(1)
    else:
        try:
            ser = serial.Serial(args.fd, timeout=0.1)
        except:
            print(f'Error: Fail to open Serial.')
            sys.exit(1)

    if args.load:
        auto_load()

    signal.signal(signal.SIGINT, sig_handler)
    t = threading.Thread(target = read_thread)
    t.start()

    while True:
        command = input()
        if command == 'exit':
            if args.load:
                send('reboot')
            is_end = True
            break

        if command == 'load':
            print("load is forbidden~\n")
            print("$ ", end="")
            continue

        if command == 'reboot':
            send('reboot')
            #print(get_until('bootloader'), end='')
            #print(get_until('$ '), end='')
            continue

        command = str.encode(command + '\n')
        ser.write(command)
        #nonblock_read()

    ser.close()
