#!python3
import sys
import serial

ser = None
#kernel = open('./kernel8.img', 'rb')

if len(sys.argv) == 2:
    ser = serial.Serial(sys.argv[1], timeout=0.1)
elif len(sys.argv) == 3:
    ser = serial.Serial(sys.argv[1], int(sys.argv[2]), timeout=0.1)

while True:
    command = input()
    if command == 'exit':
        break
    command = str.encode(command + '\n')
    ser.write(command)
    while True:
        b = ser.read(1)
        if not b:
            break
        try:
            print(b.decode('utf-8'), end='')
            sys.stdout.flush()
        except:
            print(b)

ser.close()
