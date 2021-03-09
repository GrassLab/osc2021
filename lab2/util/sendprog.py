#!/usr/bin/python3
import sys
from pathlib import Path

with open(sys.argv[1], "wb", buffering=0) as tty:
    file_size = Path(sys.argv[2]).stat().st_size
    file_size_bytes = file_size.to_bytes(8, 'little')
    with open(sys.argv[2], "rb") as file:
        tty.write(file_size_bytes)
        while True:
            data = file.read()
            if data == b'':
                break
            tty.write(data)
