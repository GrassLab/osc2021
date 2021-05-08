#!/usr/bin/env python3
from pwn import *
from time import sleep

kernel = open('./kernel8.img', 'rb').read()
r = serialtube('/dev/tty.usbserial-0001', convert_newlines=False)
#r = serialtube('/dev/pts/9', convert_newlines=False)
sleep(1)
print(r.read())
r.write(p32(len(kernel)))
print(r.read())
sleep(1)
r.write(kernel)
print(r.readuntil('kernel'))
#r.close()

#r = serialtube('/dev/tty.usbserial-0001', convert_newlines=True)
#r = serialtube('/dev/pts/9', convert_newlines=True)
r.interactive()
