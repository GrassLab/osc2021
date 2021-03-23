from serial import Serial
import argparse

parser = argparse.ArgumentParser(description='NCTU.OSDI.Lab course')
parser.add_argument('--filename', metavar='PATH', default='../kernel8.img', type=str, help='path to kernel8.img')
parser.add_argument('--device', metavar='TTY',default='/dev/ttyUSB0', type=str,  help='path to UART device')
parser.add_argument('--baud', metavar='Hz',default=115200, type=int,  help='baud rate')
parser.add_argument('--addr', metavar='s',default='0x80000', type=str,  help='load address')
args = parser.parse_args()

with open(args.filename,'rb') as fd:
    with Serial(args.device, 115200) as ser:
        kernel_raw = fd.read()
        length = len(kernel_raw)
        
        print("Kernel image size", length)
        ser.write(str(length).encode()+b'\n')
        ser.flush()


        print("Start sending kernel img by uart...")
        for i in range(length):
            # Use kernel_raw[i: i+1] is byte type. Instead of using kernel_raw[i] it will retrieve int type then cause error
            ser.write(kernel_raw[i: i+1])
            ser.flush()
            if i % 100 == 0:
                print("{:>6}/{:>6} bytes".format(i, length))
        print("{:>6}/{:>6} bytes".format(length, length))
        print("Transfer finished!")
