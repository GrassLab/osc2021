import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--path', '-p', type=str, default='kernel8.img')
parser.add_argument('--port', type=str, default='31')
args = parser.parse_args()

path = "build/{}".format(args.path)
PORT = "/dev/pts/{}".format(args.port)

size = os.path.getsize(path)
print((str(size)).encode())

with open(PORT, "wb", buffering=0) as tty:    
    #print(str(file_size)+'\n')
    tty.write((str(size)+'\0').encode())

with open(path, "rb", buffering=0) as f:
   while 1:
        with open(PORT, "wb", buffering=0) as tty:
            byte = f.read(1)
            if not byte:
                break
            tty.write(byte)

print("Done")