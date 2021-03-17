import os
import select
import serial
import sys
import time
import tty

class UartConnection:
    def __init__(self, file_path='/dev/ttyUSB0', baud_rate=115200):
        self.serial = serial.Serial(file_path, baud_rate)

    def send_string(self, string):
        return self.send_bytes(bytes(string, "ascii"))

    def send_bytes(self, bytes_to_send):
        return self.serial.write(bytes_to_send)

    def read(self, max_len):
        return self.serial.read(max_len)

    def read_buffer(self):
        return self.read(self.serial.in_waiting)

    def read_buffer_string(self):
        return self._decode_bytes(self.read_buffer())

    def start_interactive(self, input_file, output_file):
        try:
            tty.setcbreak(input_file.fileno())
            while True:
                rfd, _, _ = select.select([self.serial, input_file], [], [])

                if self.serial in rfd:
                    r = self.read_buffer_string()
                    output_file.write(r)
                    output_file.flush()

                if input_file in rfd:
                    r = input_file.read(1)
                    self.send_string(r)
        except KeyboardInterrupt:
            print("Got keyboard interrupt. Terminating...")
        except OSError:
            print("Got OSError. Terminating...")
        finally:
            os.system("stty sane")

    def _decode_bytes(self, bytes_to_decode):
        return bytes_to_decode.decode("ascii")

    def send_line(self, line):
        if not line.endswith("\n"):
            line += "\n"
        return self.send_string(line)

    def send_int(self, number):
        if number > 2 ** 32 - 1:
            raise 'Number can only be 4 bytes long'
        number_in_bytes = number.to_bytes(4, byteorder='big')
        return self.send_bytes(number_in_bytes)

    def read_int(self):
        bytes_to_read = 4
        number_bytes = self.read(bytes_to_read)
        return int.from_bytes(number_bytes, byteorder='big')

    def read_line(self):
        return self._decode_bytes(self.serial.readline())

def compute_kernel_checksum(kernel_bytes):
    num = 0
    for b in kernel_bytes:
        num = (num + b) % (2 ** 32)
    return num

def send_kernel(path, uart_connection):
    with open(path, mode='rb') as f:
        # uart_connection.send_line("kernel")
        kernel = f.read()
        # size = len(kernel)
        # checksum = compute_kernel_checksum(kernel)

        # print("Sending kernel with size", size, "and checksum", checksum)
        # uart_connection.send_int(size)
        time.sleep(1)
        # size_confirmation = uart_connection.read_int()
        # if size_confirmation != size:
        #     print("Expected size to be", size, "but got", size_confirmation)
        #     return False

        print("Kernel size confirmed. Sending kernel")
        uart_connection.send_bytes(kernel)
        time.sleep(1)
        print('Finish')
        # print("Validating checksum...")
        # checksum_confirmation = uart_connection.read_int()
        # if checksum_confirmation != checksum:
        #     print("Expected checksum to be", checksum,
        #           "but was", checksum_confirmation)
        #     return False

        # line = uart_connection.read_line()
        # if not line.startswith("Done"):
        #     print("Didn't get confirmation for the kernel. Got", line)
        #     return False

        return True

if __name__ == '__main__':
    uart_connection = UartConnection()

    time.sleep(1)
    result = send_kernel(
        path="kernel8.img",
        uart_connection=uart_connection
    )
    if result:
        print("Done!")
        uart_connection.start_interactive(sys.stdin, sys.stdout)
    else:
        print("Error sending kernel :(")