import os
import time
#file = 'test.img'
file = 'kernel8.img'

port = '/dev/ttyUSB0'
#port = '/dev/pts/6'

kernel_size = os.path.getsize(file)
index = 0

with open(port, "wb", buffering = 0) as tty:
   tty.write(("copy_loadimg\n").encode())

time.sleep(1)


with open(port, "wb", buffering = 0) as tty:
   tty.write((str(kernel_size) + '\n').encode())

print(kernel_size)
print('\0')

with open(file, "rb", buffering = 0) as tmp:
   byte = tmp.read(1)
   while(byte):
       with open(port, "wb", buffering = 0) as tty:
           index = index + 1
           print(index)
           tty.write(byte)
           print(byte)

           byte = tmp.read(1)
           



# with open(port, "wb", buffering = 0) as tty:
#    tty.write(("hello\n").encode())
#    print("It's hello!!!")

# with open(port, "wb", buffering = 0) as tty:
#    tty.write(("hello\n").encode())
#    print("It's hello!!!")