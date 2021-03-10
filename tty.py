with open('/dev/pts/4', 'wb', buffering = 0) as tty:
    with open('./build/kernel8.img', 'rb') as f:
        tty.write(f.read())
