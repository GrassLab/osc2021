import serial
import argparse

parser = argparse.ArgumentParser(description='NCTU OSDI 2021')
parser.add_argument('--filename', metavar='PATH',
                    default='./kernel/kernel8.img', type=str, help='path to script')
parser.add_argument('--device', metavar='TTY',
                    default='/dev/ttyUSB0', type=str,  help='path to UART device')
parser.add_argument('--baud', metavar='Hz', default=115200,
                    type=int,  help='baud rate')
parser.add_argument('--address', metavar='s', default='0x80000',
                    type=str,  help='load address')
args = parser.parse_args()

with open(args.filename, 'rb') as file:
    with serial.Serial(args.device, args.baud) as device:
        
        raw = file.read()
        length = len(raw)
        length_sent = len(raw)

        while length_sent > 0:
            
            digit = length_sent % 10
            length_sent = length_sent // 10
            
            device.write(str(digit).encode())

        device.write(str('\n').encode())

        for i in range(length):
            device.write(raw[i: i+1])
            print(i)
