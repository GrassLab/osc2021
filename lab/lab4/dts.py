import struct

filename = "./bcm2710-rpi-3-b-plus.dtb"


def is_ascii(s):
    for c in s:
        if c >= 128:
            return False
    return True


dtb2 = open(filename, "r")
with open(filename, "rb") as dtb:
    # fdt_header
    magic = int.from_bytes(dtb.read(4), byteorder='big')
    totalsize = int.from_bytes(dtb.read(4), byteorder='big')
    off_dt_struct = int.from_bytes(dtb.read(4), byteorder='big')
    off_dt_strings = int.from_bytes(dtb.read(4), byteorder='big')
    off_mem_rsvmap = int.from_bytes(dtb.read(4), byteorder='big')
    version = int.from_bytes(dtb.read(4), byteorder='big')
    last_comp_version = int.from_bytes(dtb.read(4), byteorder='big')
    boot_cpuid_phys = int.from_bytes(dtb.read(4), byteorder='big')
    size_dt_strings = int.from_bytes(dtb.read(4), byteorder='big')
    size_dt_struct = int.from_bytes(dtb.read(4), byteorder='big')

    # get string block
    dtb2.seek(off_dt_strings, 0)
    # i = 0
    # c = dtb2.read(1)
    # while c:
    #     while c != "\0":
    #         print(c, end='')
    #         c = dtb2.read(1)
    #     print()
    #     c = dtb2.read(1)
    # exit()

    # fdt_reserve_entry
    while True:
        address = int.from_bytes(dtb.read(8), byteorder='big')
        size = int.from_bytes(dtb.read(8), byteorder='big')
        if address == size == 0:
            break

    # NODES
    tmp_arr = []
    deep = -1
    while True:
        FDT_NODE = int.from_bytes(dtb.read(4), byteorder='big')
        if FDT_NODE == int("0x00000001", 16):
            for i in range(deep):
                print("+", end='')
            print("name: ", end='')
            name = dtb.read(1).decode('ascii')
            reminder = 1
            while name != "\0":
                print(name, end='')
                name = dtb.read(1).decode('ascii')
                reminder += 1
            print()
            deep += 1
            reminder = (4 - (reminder % 4)) % 4
            dtb.read(reminder)

        elif FDT_NODE == int("0x00000002", 16):
            deep -= 1

        elif FDT_NODE == int("0x00000003", 16):
            # FDT_PROP struct
            FDT_PROP_len = int.from_bytes(dtb.read(4), byteorder='big')
            FDT_PROP_nameoff = int.from_bytes(dtb.read(4), byteorder='big')
            reminder = (4 - (FDT_PROP_len % 4)) % 4
            value = dtb.read(FDT_PROP_len + reminder)
            for i in range(deep):
                print(" ", end='')
            # print()
            # if FDT_PROP_nameoff == 0:
            #     print("compatible: ", value[:-reminder-1])
            # elif FDT_PROP_nameoff == 1:
            #     print(": #address-cells, #size-cells: ", hex(value[:-reminder]))
            # if FDT_PROP_nameoff == 0:

        elif FDT_NODE == int("0x00000004", 16):
            continue
        elif FDT_NODE == int("0x00000009", 16):
            break
