import os

PORT = '/dev/pts/25'
prog_path = 'kernel8.img'
file_size = os.path.getsize(prog_path) # bytes
#print(file_size)
#print(file_size)

with open(PORT, "wb", buffering=0) as tty:    
    #print(str(file_size)+'\n')
    tty.write((str(file_size)+'\n').encode())

index = 0
with open(prog_path, "rb") as f:
    byte = f.read(1)
    while(byte):
        with open(PORT, "wb", buffering=0) as tty:
            index = index + 1
            #print(index)
            tty.write(byte)
            #print(byte)

            byte = f.read(1)
print(file_size)

