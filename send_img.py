import sys
import os
import numpy as np
import time

file_path = ''
device_address = ''

def get_sum(bytedata):
    return int(np.array(list(bytedata), dtype=np.int32).sum())

def send_img():
    file = open(file_path, 'rb')
    bytedata = file.read()
    file.close()

    with open(device_address, 'wb') as tty:
        file_size = os.stat(file_path).st_size
        tty.write(file_size.to_bytes(4, byteorder="big"))
        checksum = get_sum(bytedata)
        tty.write(checksum.to_bytes(5, byteorder="big"))
    time.sleep(1)
    with open(device_address, 'wb') as tty:
        tty.write(bytedata)

if __name__ == "__main__":
    parameter = sys.argv
    if len(parameter) != 3:
        print('Usage: [Device address] [File path]')
        sys.exit()
    device_address = parameter[1]
    file_path = parameter[2]
    send_img()
