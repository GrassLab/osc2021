with open('/dev/pts/5', 'wb', buffering = 0) as tty,  open('./build/kernel8.img', 'rb') as f:
    tty.write('s'.encode())
    tty.write(f.read())
    tty.write('end'.encode())