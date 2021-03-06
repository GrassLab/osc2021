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
        print('Section :', section.name)
        serial.send('S')
        res = b''
        while len(res) < 2 or res[-2:] != b'ST':
            res = serial.recvuntil('ST', timeout = 3)
            print(res)
        print('Address :', hex(section.header.sh_addr))
        serial.send(pwn.p64(section.header.sh_addr))
        print('Size :', hex(section.data_size))
        serial.send(pwn.p32(section.data_size))
        checksum = 0
        prev_checksum = 0
        print('Type :', section.header.sh_type)
        if (section.header.sh_type == 'SHT_NOBITS'):
            serial.send('A')
        else:
            serial.send('D')
            # serial.send(kernel_bin.section(section.name))
            sec = kernel_bin.section(section.name)
            c = 11
            while c < len(sec):
                checksum = c
                # checksum = sec[c]
                serial.send(pwn.p8(c))
                # serial.send(pwn.p8(sec[c]))
                c += 1
                if(True):
                    check = serial.recv(1)[0]
                    serial.send(pwn.p8(0x69))
                    
                    # print(serial.recv(timeout=0.1))
                    if(check == checksum):
                        prev_checksum = checksum
                        serial.send(pwn.p8(0))
                        print("Check ", c, checksum, check)
                    else:
                        print("Revert ", c, checksum, check)
                        serial.send(pwn.p8(1))
                        c -= 1
                        checksum = prev_checksum
                        time.sleep(1)
                
        res = serial.recvuntil('FN')
        # print(res)
        print('Checksum :', checksum, res[-7])
        print('Total Transerfer :', hex(pwn.u32(res[-6: -2])))
        print('Finish')

serial.send('K')
print('Entry Point :', hex(kernel_bin.entry))
serial.send(pwn.p64(kernel_bin.entry))
serial.send('E')

serial.interactive()
