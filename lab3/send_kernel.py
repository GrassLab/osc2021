import time
import os
import struct

tty_path = '/dev/pts/3'
#tty_path = '/dev/ttyUSB0'
kernel_path = 'kernel8.img'
size = os.path.getsize(kernel_path)

with open(tty_path, "wb", buffering = 0) as tty:
  tty.write(struct.pack('>I', size))
  print('kernel size: %s' % size)
  time.sleep(1)

  with open(kernel_path, "rb") as kernel:
    write_len = tty.write(kernel.read())
    print("write %d bytes" % write_len)
