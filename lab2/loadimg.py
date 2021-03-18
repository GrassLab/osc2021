#! /usr/bin/python3
import os
import time
import serial#pyserial

def waitFor(target,dev,display=True):#avoid loss data
    msgs=''
    while True:
        cnt=dev.inWaiting()
        if cnt>0:
            msg=dev.read(cnt).decode()
            msgs=msgs+msg
            if display:
                print(msg,end='')
            if msgs.find(target)!=-1:
                return

def dumpImg(dev,k_addr='0xa0000',kernel='kernel87.img'):
    k_size=os.stat(kernel).st_size

    waitFor(': ',dev)
    dev.write(str.encode(k_addr+'\n'))

    waitFor(': ',dev)
    dev.write(str.encode(str(k_size)+'\n'))

    with open(kernel,"rb") as f:
        waitFor('...\n',dev)
        for i in range(k_size):
            dev.write(f.read(1))
            waitFor('.',dev,False)

if __name__=='__main__':
    br=115200
    dev=serial.Serial("/dev/ttyUSB0",br)

    test_case='./input'
    f=open(test_case,'r')
    while True:
        #input
        cmd=f.readline()
        if cmd=='':
            break
        dev.write(str.encode(cmd))
        if cmd=='loadimg\n':
            k_addr=f.readline()
            kernel=f.readline()
            dumpImg(dev,k_addr=k_addr[:-1],kernel=kernel[:-1])

        #output
        waitFor('~ ',dev)
