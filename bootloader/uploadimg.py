import serial
import os
import sys
import numpy as np
import time

image_path = sys.argv[1]
tty_path = sys.argv[2]


def checksum(bytecodes):
    # convert bytes to int
    return int(np.array(list(bytecodes), dtype=np.int32).sum())


def main():
    print(tty_path)
    try:
        ser = serial.Serial(tty_path, 115200)
    except:
        print("Serial init failed!")
        exit(1)

    file_path = image_path
    file_size = os.stat(file_path).st_size

    with open(file_path, 'rb') as f:
        bytecodes = f.read()

    file_checksum = checksum(bytecodes)

    ser.write(file_size.to_bytes(4, byteorder="big"))
    ser.write(file_checksum.to_bytes(4, byteorder="big"))

    print(f"Image Size: {file_size}, Checksum: {file_checksum}")

    per_chunk = 128
    chunk_count = file_size // per_chunk
    chunk_count = chunk_count + 1 if file_size % per_chunk else chunk_count

    for i in range(chunk_count):
        sys.stdout.write('\r')
        sys.stdout.write("%d/%d" % (i + 1, chunk_count))
        sys.stdout.flush()
        ser.write(bytecodes[i * per_chunk: (i+1) * per_chunk])
        time.sleep(0.05)
        while not ser.writable():
            pass
    print()


if __name__ == "__main__":
    main()
