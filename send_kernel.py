import pwn
import time
from functools import reduce

def send_kernel(serial, kernel_file):
    print('Kernel :')
    # transfer kernel binary
    serial.send('s')

    kernel_bin = pwn.ELF(kernel_file)

    for section in kernel_bin.sections:
        if(section.__class__.__name__ == 'Section' and 'debug' not in section.name):
            checksum = 0
            res_checksum = 1
            while checksum != res_checksum:
                start_time = time.time()
                print('\nSection :', section.name)
                serial.send('S')
                serial.recvuntil('S')

                print('Address :', hex(section.header.sh_addr))
                serial.send(pwn.p64(section.header.sh_addr))

                print('Size :', hex(section.data_size))
                serial.send(pwn.p64(section.data_size))

                checksum = 0
                print('Type :', section.header.sh_type)
                if (section.header.sh_type == 'SHT_NOBITS'):
                    serial.send('A')
                elif (section.header.sh_type == 'SHT_PROGBITS'):
                    serial.send('D')
                    section_bin = kernel_bin.section(section.name)
                    serial.send(section_bin)
                    checksum = reduce(lambda x, y: x ^ y, section_bin)
                res_checksum = pwn.u8(serial.recv(1))
                print('Checksum :', checksum == res_checksum)
                print(f'Elapse Time :{time.time() - start_time:.2f}s')

    serial.send('E')
    print('\nEntry Point :', hex(kernel_bin.entry))
    serial.send(pwn.p64(kernel_bin.entry))
