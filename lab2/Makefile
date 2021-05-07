LD = aarch64-linux-gnu-ld
LDFLAGS = -T linker.ld

BUILD_DIR = build
SRC = $(wildcard src/*.S) $(wildcard src/*.c) $(wildcard lib/*.c)
OBJ = start.o $(filter-out start.o, $(wildcard *.o))
INC = $(wildcard lib/)

KERNEL = kernel8
ELF = $(KERNEL).elf
IMG = $(KERNEL).img

CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary
OD = aarch64-linux-gnu-objdump

GDB = aarch64-linux-gnu-gdb
GDBFLAGS = -x ./debug.gdb

all:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC) -c -g $(SRC)
	make kernel
	$(MAKE) -C bootloader

kernel:
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(ELF) $(OBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(ELF) $(BUILD_DIR)/$(IMG)
	$(OD) -D $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/$(KERNEL).objdump
	$(OD) -x $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/$(KERNEL).xobjdump
	$(OD) -s $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/$(KERNEL).content
	rm $(OBJ)


clean:
	rm -rf $(BUILD_DIR)

debug:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(KERNEL).img -display none -S -s -initrd initramfs.cpio

gdb:
	$(GDB) $(GDBFLAGS)

run:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(KERNEL).img -display none -serial null -serial stdio -initrd initramfs.cpio

test: 
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/bootloader.img -serial null -serial pty -initrd initramfs.cpio