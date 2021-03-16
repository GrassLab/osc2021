import serial
import os
import argparse
import time

parser = argparse.ArgumentParser(description='Send kernel to raspi')
parser.add_argument('--file', type=str, help='kernel file.')
parser.add_argument('--port', type=str, help='which io port to send.')
args = parser.parse_args()

kernel_file = os.path.join(args.file)
# # uart_path = os.path.join('/dev', 'pts/4')
uart_path = os.path.join('\\.','\COM3')
kernel_size = os.path.getsize(args.file)

# srl = serial.Serial(port=uart_path, baudrate=115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1)
srl = serial.Serial(port='\\.\COM3', baudrate=115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=0) 
for c in str(kernel_size):
    srl.write(str.encode(c))
srl.write(str.encode("\n"))

with open(kernel_file, "rb") as fp:
    byte = fp.read(1)
    while byte:
        srl.write(byte)
        # time.sleep(0.001)
        byte = fp.read(1)

print("Transmit new kernel done\n")
srl.close()