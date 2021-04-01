#!/usr/bin/env python3

import os
import serial
from time import sleep


def screenIO(tty_dev):
    with serial.Serial(tty_dev, 115200, timeout=1) as ser:
        #ser.open()
        while True:
            cmd = input()
            if cmd == "quit" or cmd == "q":
                break
            ser.write(bytes(cmd, "UTF-8"))
            ser.write(bytes("\n", "UTF-8"))
            sleep(0.05)
            if cmd == "load":
                load_binary(tty_dev)
                sleep(0.05)
                break
            bytesToRead = ser.inWaiting()
            c = ser.read(bytesToRead)
            print(c.decode("UTF-8"), end='')
        ser.close()


def load_binary(tty_dev):
    boot_file_path = input("Input Boot image file[../kernel8.img]: ")
    if boot_file_path == "":
        boot_file_path = "../kernel8.img"
    file_size = os.path.getsize(boot_file_path)
    if file_size > 0xffffffff:
        print("Kernel file too large!!!")
        return
    boot_file = open(boot_file_path, "rb")

    with open(tty_dev, "wb") as tty:
        tty.write(file_size.to_bytes(4, byteorder="big", signed=False))
        tty.close()
    with open(tty_dev, "wb", buffering = 0) as tty:
        tty.write(boot_file.read())
        tty.close()
    with open(tty_dev, "w") as tty:
        tty.write("c\n")
        tty.close()


def main():
    tty_dev = input("Input tty device: ")
    screenIO(tty_dev)


if __name__ == "__main__":
    main()
