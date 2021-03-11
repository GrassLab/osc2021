import pty
import time
import os
import sys

kernel_file = "test/kernel9.img"
with open(kernel_file, 'rb') as f:
    kernel = f.read()

kernel_size = len(kernel)

num = 0
check_arr = []
for b in kernel:
    num += b
    check_arr.append(b)
print("size is: ", kernel_size)
print("checksum is: ", num)

pid, fd = pty.fork()
if pid == 0:
    # Child process
    while True:
        try:
            #sys.stdout.write('Hello World!\n')
            #os.write(fd, 'hello'.encode())
            time.sleep(100)
        except KeyboardInterrupt:
            sys.stdout.write('SIGINT Received!\n')
            sys.exit(1)
else:
    os.write(fd, kernel)
    print('Parent wait for 1 sec then write 0x03...')
    time.sleep(1)
    print('Parent write 0x03')
    os.write(fd, b'\x03')
    # Read until EOF or Input/Output Error
    data = b''
    while True:
        try:
            buf = os.read(fd, 1)
        except OSError:
            break
        else:
            if buf != b'':
                data += buf
            else:
                break
        time.sleep(0.01)

    print("reading done")
    num = 0
    result_arr = []
    for b in data:
        num += b
        result_arr.append(b)
    print(num)
    
    for i in range(100):
        if(check_arr[i] != result_arr[i]):
            print(i, check_arr[i], result_arr[i])

    #print('Parent read from pty fd: {}'.format(repr(data)))
    print('Parent wait for child process {!r} to exit...'.format(pid))
    pid, status = os.waitpid(pid, 0)
    print('Parent exit')

    
