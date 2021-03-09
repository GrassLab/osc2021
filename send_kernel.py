import serial
import os

kernel_file = os.path.join('test_kernel','kernel8_test2.img')
#uart_path = os.path.join('/dev', 'pts/2')
uart_path = os.path.join('\\.','\COM3')
kernel_size = os.path.getsize(kernel_file)

# srl = serial.Serial(port=uart_path, baudrate=115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1)
srl = serial.Serial(port='\\.\COM3', baudrate=115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1) 
for c in str(kernel_size):
    srl.write(str.encode(c))
srl.write(str.encode("\n"))

with open(kernel_file, "rb") as fp:
    byte = fp.read(1)
    while byte:
        srl.write(byte)
        byte = fp.read(1)

print("Transmit new kernel done\n")
srl.close()