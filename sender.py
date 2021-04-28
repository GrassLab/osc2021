from pwn import *
from time import sleep

kernel = open('./kernel/kernel8.img', 'rb').read()

print("Start sending img...")

#device = '/dev/ttys004'
device = '/dev/cu.SLAB_USBtoUART'

with open(device, "wb", buffering = 0) as tty:
    tty.write(p32(len(kernel))) #send bytes for littile endian
    print('Send size ok!')
    sleep(1)
    tty.write(kernel)
    print('Send img ok!')

print("Finish!")