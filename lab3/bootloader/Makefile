LD = aarch64-linux-gnu-ld
LDFLAGS = -T linker.ld

BUILD_DIR = ../build
SRC = $(wildcard src/*.S) $(wildcard ../lib/*.c) $(wildcard src/*.c) $(wildcard ../3rdparty/*)
OBJ = boot.o $(filter-out boot.o, $(wildcard *.o))
INC = $(wildcard ../lib/)

BOOT = bootloader
ELF = $(BOOT).elf
IMG = $(BOOT).img

CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary
OD = aarch64-linux-gnu-objdump

GDB = aarch64-linux-gnu-gdb
GDBFLAGS = -x ./debug.gdb

all:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I../3rdparty -I$(INC) -c -g $(SRC)
	make bootloader

bootloader:
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(ELF) $(OBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(ELF) $(BUILD_DIR)/$(IMG)
	$(OD) -D $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/$(BOOT).objdump
	$(OD) -x $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/$(BOOT).xobjdump
	$(OD) -s $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/$(BOOT).content
	rm $(OBJ)

debug:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(BOOT).img -display none

gdb:
	$(GDB) $(GDBFLAGS)

run:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(BOOT).img -display none -serial null -serial stdio -initrd initramfs.cpio

test: 
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(BOOT).img -serial null -serial pty -initrd ../initramfs.cpio