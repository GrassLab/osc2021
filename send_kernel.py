import sys

with open('/dev/ttyUSB0', 'rb+', buffering = 0) as tty:
    #with open('KERNEL8.IMG', 'rb') as kernel_file:
    with open('kernel8.img', 'rb') as kernel_file:
        kernel_data = kernel_file.read()

        print('Start to send kernel img...')
        
        tty.write('s'.encode())

        tty.write(len(kernel_data).to_bytes(4, 'little'))

#        print(len(kernel_data).to_bytes(4, 'little')[1])
        tty.write(kernel_data)

        print('......Done!')
