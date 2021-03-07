import pwn
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument('-t', '--tty', help='target tty', default='/dev/ttyUSB0')
parser.add_argument('-b', '--baudrate',
                    help='serial baudrate', type=int, default=115200)
parser.add_argument('-k', '--kernel', help='kernel file',
                    default='kernel8.elf')
args = parser.parse_args()

tty = args.tty
baudrate = args.baudrate
kernel_file = args.kernel

serial = pwn.serialtube(tty, baudrate=baudrate)

# transfer kernel binary
serial.send('s')

kernel_bin = pwn.ELF(kernel_file)
for section in kernel_bin.sections:
    if(section.__class__.__name__ == 'Section' and 'debug' not in section.name):
        print('\nSection :', section.name)
        serial.send('S')
        serial.recvuntil('S')

        print('Address :', hex(section.header.sh_addr))
        serial.send(pwn.p64(section.header.sh_addr))

        print('Size :', hex(section.data_size))
        serial.send(pwn.p32(section.data_size))

        checksum = 0

        print('Type :', section.header.sh_type)
        if (section.header.sh_type == 'SHT_NOBITS'):
            serial.send('A')
        else:
            serial.send('D')
            sec = kernel_bin.section(section.name)
            # cnt = 0
            for d in sec:
                if(d == 10 or d == 13):
                    serial.send(pwn.p8(13))
                    serial.send(pwn.p8(13 - d))
                else:
                    serial.send(pwn.p8(d))
                checksum ^= d
        res_checksum = pwn.u8(serial.recv(1))
        serial.recvuntil('FN')
        print('Checksum :', checksum == res_checksum)

serial.send('K')
print('Entry Point :', hex(kernel_bin.entry))
serial.send(pwn.p64(kernel_bin.entry))
serial.send('E')

serial.interactive()
