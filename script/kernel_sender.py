#!/usr/bin/env python3
from pwn import *
from time import sleep

kernel = open('./kernel8.img', 'rb').read()

print("Start transmitting ...")

with open('/dev/ttyUSB0', "wb", buffering = 0) as tty:
    tty.write(p32(len(kernel)))
    sleep(1)
    tty.write(kernel)

print("Finish!")

