import time
import os

kernel_file = 'test/kernel9.img'
kernel_size = os.path.getsize(kernel_file)

with open(kernel_file, 'rb') as kernel_f:    
    kernel = kernel_f.read()
    kernel_f.close()
'''
num = 0
for b in kernel:
    num += b
print(num)
'''

with open('/dev/ttyUSB0', 'wb', buffering=0) as tty:
    # command
    tty.write(b'loadimg\n')
    time.sleep(1)
    # send kernel size        
    tty.write((str(kernel_size) + '\n').encode())
    time.sleep(1)

    # write the binary file to tty
    tty.write(kernel)
    time.sleep(1)

print('image done!')


