import os
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument('--path', '-p', type=str, default='kernel8.img')
parser.add_argument('--port', type=str, default='31')
args = parser.parse_args()

path = "build/{}".format(args.path)
#PORT = "/dev/pts/{}".format(args.port)
PORT = "/dev/ttyUSB0"

size = os.path.getsize(path)
print((str(size)).encode())

with open(PORT, "wb", buffering=0) as tty:    
    #print(str(file_size)+'\n')
    tty.write((str(size)+'\0').encode())

time.sleep(1)

#flag = 0
with open(path, "rb", buffering=0) as f:
   while 1:
        with open(PORT, "wb", buffering=0) as tty:
            byte = f.read(1)
            if not byte:
                break
            tty.write(byte)
            time.sleep(0.0001)
        #print(flag)
        #flag += 1
        #time.sleep(0.001)

print("Done")