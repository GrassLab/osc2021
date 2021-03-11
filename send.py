with open('/dev/pts/4', "wb", buffering = 0) as tty:
    with open('./bootloader.img', "rb") as fp:
        c = fp.read(1)
        while c:
            tty.write(c)
            c = fp.read(1)

fp.close()
tty.close()