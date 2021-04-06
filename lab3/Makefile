CXX = aarch64-linux-gnu-gcc
CFLAGS = -Ilib -Itest -Ielibc -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles 

LD = aarch64-linux-gnu-ld
LDFLAGS = -T lib/linker.ld

OD = aarch64-linux-gnu-objdump

OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary

GDB = aarch64-linux-gnu-gdb
GDBFLAGS = -x ./debug.gdb

BUILD_DIR = build
PROJ = kernel8
ELF = $(PROJ).elf
IMG = $(PROJ).img
EBF = $(PROJ).ebf
SRC = $(wildcard test/*.S) $(wildcard test/*.c) $(wildcard lib/*.c) $(wildcard elibc/*.c)
OBJ = $(wildcard *.o)
#boot.o $(filter-out boot.o, $(wildcard *.o))


all:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CFLAGS) -c -g $(SRC)
	make kernel8

kernel:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CFLAGS) -c $(SRC) -D BOOT_ENABLE
	make kernel8

kernel8:
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(ELF) $(OBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(ELF) $(BUILD_DIR)/$(IMG)
	$(OD) -D $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/kernel8.objdump
	$(OD) -x $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/kernel8.xobjdump
	$(OD) -s $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/kernel8.content
	rm $(OBJ)
	make ebf
	
ebf:
	./create_ebf.sh $(BUILD_DIR)/$(IMG)

debug:
	qemu-system-aarch64 -M raspi3\
		-kernel $(BUILD_DIR)/$(IMG)\
		-display none\
		-serial null\
		-serial stdio\
		-S -s

run:
	qemu-system-aarch64 -M raspi3\
		-kernel $(BUILD_DIR)/$(IMG)\
		-display none\
		-serial null\
		-serial stdio\
		-initrd initramfs.cpio

gdb:
	$(GDB) $(GDBFLAGS)

clean:
	rm -rf $(BUILD_DIR)/*
	rm $(OBJ)
	rm $(wildcard *.bin)


