import sys
import threading
import serial



if len(sys.argv) > 1:
    s = serial.Serial(
        port = sys.argv[1],
        baudrate = 115200, 
        bytesize = 8
    )

    def listen():
        while 1:
            if s.in_waiting > 0:
                print(s.read().decode('ascii'), end = '')
                global stop_threads
                if stop_threads:
                    break

    t = threading.Thread(target = listen)
    stop_threads = False
    t.start()

    with open('./build/kernel8.img', 'rb') as f:   
        s.write('start'.encode())
        s.write(f.read())
        s.write('end'.encode())

    t.join()
else:
    print("Failed, please specify a device path")