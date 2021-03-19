import argparse
from pathlib import Path
import serial

def send_all_data(tty, data):
    while len(data) > 0:
        write_cnt = tty.write(data)
        data = data[write_cnt:]

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-s", 
        "--src_file_pth",
        type=str, 
        help='source file path',
        default='../kernel8.img'
    )
    parser.add_argument(
        "-t", 
        "--target_pth", 
        nargs='?', 
        type=str, 
        help='target path', 
        default='/dev/ttyUSB0'
    )
    args = parser.parse_args()
    
    with serial.Serial(args.target_pth, 115200, xonxoff=True) as tty:
        magic = 0x01020304.to_bytes(4, 'big')
        send_all_data(tty, magic)

        file_size = Path(args.src_file_pth).stat().st_size.to_bytes(8, 'little')
        send_all_data(tty, file_size)

        with open(args.src_file_pth, 'rb') as src_file:
            while True:
                data = src_file.read()
                if data == b'':
                    break
                send_all_data(tty, data)
        print("finish sending kernel8.img\n")