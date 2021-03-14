import os
kernel_size = os.path.getsize('kernel8.img')
index = 1




with open('kernel8.img', "rb", buffering = 0) as tmp:
    byte = tmp.read(1)
    while(byte):
        print(byte)
        byte = tmp.read(1)





