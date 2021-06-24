#! python2

import os
import time
import serial

filename = "./kernel/kernel8.img"
ser_port = "/dev/ttyS4"
ser_baudrate = 115200

try:    
    ser = serial.Serial(
        port=ser_port,
        baudrate=ser_baudrate,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )
    print("Open Serial Port\r") 
    print(f"Port=%s, baudrate=%d\r" % (ser_port, ser_baudrate))

    with open(filename, 'rb') as file:
        
        size = os.path.getsize(filename)
        print(f"Image size is {size}")
        ser.write(f"{size}\r".encode())
        
        time.sleep(2)

        position = 0
        byte = file.read(1)        
        while byte != b"":
            file.seek(position, 0)
            byte = file.read(1)
            ser.write(byte)
            position = position + 1

        print("Done !")  
        file.close()

except Exception as e:
    print(f"[EXCEPTION] {e}")

