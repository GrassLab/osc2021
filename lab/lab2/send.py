from os.path import getsize, join
import sys
from time import sleep
from sys import platform
from serial import Serial


kernel_filename = "./kernel.img"
device = int(sys.argv[1])

if platform == "linux" or platform == "linux2":
    if device != -1:
        device = "/dev/pts/%d" % device
    else:
        device = "/dev/ttyUSB0"
    rpi3 = Serial(device, 115200, timeout=0.5)
    # with open(device, "wb", buffering=0) as rpi3:
    with open(kernel_filename, "rb") as kernel:
        # send kernel size
        kernel_size = getsize(kernel_filename)
        rpi3.write(("%d\0" % kernel_size).encode())
        print("kernel size: %d" % kernel_size)
        sleep(0.5)
        # read and send kernel.img
        for i in range(kernel_size):
            c = kernel.read(1)
            rpi3.write(c)
            print("%d / %d" % (i+1, kernel_size), end="\r")
            sleep(0.001)
    rpi3.close()
else:
    device = join("COM%d" % device)
    rpi3 = Serial(device, 115200, timeout=0.5)
    # send kernel size
    kernel_size = getsize(kernel_filename)
    rpi3.write(("%d\0" % kernel_size).encode())
    print("kernel size: %d" % kernel_size)
    sleep(0.5)
    with open(kernel_filename, "rb") as kernel:
        # read and send kernel.img
        for i in range(kernel_size):
            c = kernel.read(1)
            rpi3.write(c)
            print("%d / %d" % (i+1, kernel_size), end="\r")
            sleep(0.125)
    rpi3.close()
