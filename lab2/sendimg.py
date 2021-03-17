import sys
import os
import struct
from serial import Serial, SerialException
from serial.tools import miniterm

def main():
    if len(sys.argv) != 3:
        print('Usage: python3 {} <port_num> <image_path>'.format(sys.argv[0]))
        sys.exit()

    input('press enter to start')

    try:
        con = Serial(sys.argv[1], BAUDRATE)
    except SerialException as e:
        print(e)
        sys.exit()

    try:
        size = os.path.getsize(sys.argv[2])
    except FileNotFoundError as e:
        print(e)
        sys.exit()

    if size > 10000:
        print('kernel image exceed 10kb ({}byte)'.format(size))
        sys.exit()

    # print(con.readline()) #welcome msg
    # print(con.readline()) #welcome msg2
    con.write(b'Q')

    con.write(struct.pack('<L', size))
    recv = struct.unpack('<L', con.read(4))[0]
    if recv != size:
        print('handshake failed. s:{}byte, r:{}byte'.format(size, recv))
        sys.exit()

    with open(sys.argv[2], "rb") as image:
        for i in range(size):
            con.write(image.read(1))
    con.close()
    print('done')

BAUDRATE = 115200

if __name__ == '__main__':
    main()
