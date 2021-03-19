#!/usr/bin/env python3
import sys
from pathlib import Path
from serial import Serial

with Serial(sys.argv[1], 115200, xonxoff=True) as tty:
    magic = 0x01020304
    magic_byte = magic.to_bytes(4, 'big')
    while len(magic_byte) > 0:
        write_count = tty.write(magic_byte)
        magic_byte = magic_byte[write_count:]
    file_size = Path(sys.argv[2]).stat().st_size
    file_size_bytes = file_size.to_bytes(8, 'little')
    while len(file_size_bytes) > 0:
        write_count = tty.write(file_size_bytes)
        file_size_bytes = file_size_bytes[write_count:]
    with open(sys.argv[2], "rb") as file:
        tty.write(file_size_bytes)
        while True:
            data = file.read()
            if data == b'':
                break
            while len(data) > 0:
                write_count = tty.write(data)
                data = data[write_count:]
