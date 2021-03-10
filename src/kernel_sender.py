import sys
if len(sys.argv) > 1:
    with open(sys.argv[1], 'wb', buffering = 0) as tty,  open('./build/kernel8.img', 'rb') as f:
        tty.write('s'.encode())
        tty.write(f.read())
        tty.write('end'.encode())
else:
    print("Failed, please specify a device path")