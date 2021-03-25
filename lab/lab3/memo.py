'''
memory address
0x0000
|     |     spin table
0x1000
|     |
|     |
0x80000     bootloader
|     |
|     |
0x180000    new bootloader
|     |
|     |
0x400000    kernel
|     |
|     |
0x8000000   initramfs
|     |
|     |
0x9000000   *deviceTree
|     |
|     |
|     |
.     .
.     .
.     .
|     |

DeviceTree???
'''
'''
memory address
0x0000
|     |     spin table
0x1000
|     |
|     |
0x80000
|     |     bootloader --> kernel
|     |     4MB
0x480000
|     |     new bootloader --> kernel tables
|     |     4MB
0x880000    *deviceTree
|     |
|     |
.     .
|     |
0x8000000   
|     |     initramfs
|     |     4MB
|     |
.     .
|     |
0x10000000  alloc_start
|     |
|     |
.     .
.     .
|     |
0x40000000  alloc_end

DeviceTree???
'''
# requirement 1
memIdx = {0: Priority_queue(key=address),  # 4KB
          1: Priority_queue(key=address),  # 8KB
          2: Priority_queue(key=address),  # 16KB
          3: Priority_queue(key=address),  # 32KB
          4: Priority_queue(key=address),  # 64KB
          5: Priority_queue(key=address),  # 128KB
          6: Priority_queue(key=address),  # 256KB
          7: Priority_queue(key=address),  # 512KB
          8: Priority_queue(key=address),  # 1MB
          8: Priority_queue(key=address),  # 2MB
          10: Priority_queue(key=address)}  # 4MB

start_address
end_address


def split(addr, size):
    return addr, addr+size


def page_alloc(size):
    index = log2(size)
    if memIdx[index].empty():
        addr = page_alloc(2 * size)
        addr0, addr1 = split(addr)
        memIdx[index].push(addr0)
        memIdx[index].push(addr1)
    return memIdx[index].pop()


def mem_init(memIdx):
    max_page_size = 8MB
    page_sizes = reversed([2**i * 4KB for i in range(10)])
    for page_size in page_sizes:
        for i in range(init_address, end_address, page_size):
            memIdx[10].push(i)
        init_address = i + page_size

# requirement 2
