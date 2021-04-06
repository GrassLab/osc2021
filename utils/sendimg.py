import argparse
import serial
import os
import sys

parser = argparse.ArgumentParser()
parser.add_argument("image_file")
parser.add_argument("tty")

args = parser.parse_args()

def main():
    image_path = args.image_file
    image_size = os.stat(image_path).st_size

    tty_session = None

    try:
        tty_session = serial.Serial(args.tty, 115200)
    except Exception as e:
        print("tty init error.")
        print(e)
        exit(1)

    with open(image_path, "rb") as f:
        img_bytecode = f.read()

    # enter loadimg
    while not tty_session.writable():
        pass
    tty_session.write(bytes("loadimg\r", encoding="ascii"))
    tty_session.flush()

    print("enter loadimg")
    sys.stdout.flush()

    # send size first
    while not tty_session.writable():
        pass
    tty_session.write(bytes(str(image_size)+"\r", encoding="ascii"));
    tty_session.flush()

    print("enter size")
    sys.stdout.flush()

    per_chunk = 1
    chunk_count = image_size // per_chunk
    if image_size % per_chunk != 0:
        chunk_count += 1
    # send binary second
    for i in range(chunk_count):
        sys.stdout.write("{}/{}\n".format(i+1, chunk_count))
        sys.stdout.flush()
        tty_session.write(img_bytecode[i*per_chunk:(i+1)*per_chunk])
        tty_session.flush()
        while not tty_session.writable():
            pass


if __name__ == "__main__":
    main()
