#! /usr/bin/env python3
import argparse
import os
import serial


parser = argparse.ArgumentParser()

parser.add_argument("-f", "--filename", help="the input file")
parser.add_argument("--tty", help="the target tty")
parser.add_argument("-b", "--baudrate", help="serial baudrate", default=115200)

args = parser.parse_args()

# def bytes_from_file(filename, chunksize=65536):
#     with open(filename, "rb") as f:
#         chunk = f.read(chunksize)
#         return chunk

with serial.Serial(args.tty, args.baudrate) as ser:
    size = os.path.getsize(args.filename)
    print(f'Image size is {size}')
    res = f"{size}\n"
    ser.write(res.encode())

    with open(args.filename, "rb") as f:
        chunk = f.read(8192)
        while chunk:
            ser.write(chunk)
            chunk = f.read(8192)

    print("done")
