import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-s", 
        "--src_file_pth",
        type=str, 
        help='source file path'
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
    
    with open(args.target_pth, 'wb', buffering=0) as tty, open(args.src_file_pth, 'rb') as src_file:
        while True:
            data = src_file.read()
            if data == b'':
                break
            tty.write(data)