import pwn
import argparse
from send_kernel import *

parser = argparse.ArgumentParser()
parser.add_argument('-t', '--tty', help='target tty', default='/dev/ttyUSB0')
parser.add_argument('-b', '--baudrate',
                    help='serial baudrate', type=int, default=115200)
parser.add_argument('-k', '--kernel', help='kernel file',
                    default='kernel8.elf')
args = parser.parse_args()

tty = args.tty
baudrate = args.baudrate
kernel_file = args.kernel

serial = pwn.serialtube(tty, baudrate=baudrate,  convert_newlines=False)

send_kernel(serial, kernel_file)

serial.interactive()
