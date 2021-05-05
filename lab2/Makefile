

LD = aarch64-linux-gnu-ld
LDBOOTFLAGS = -T linkerBoot.ld
LDKERNELFLAGS = -T linker.ld

BUILD_DIR = build
SRCS = $(wildcard src/*.c) $(wildcard src/*.S) $(wildcard lib/*.c)
OBJS = $(wildcard *.o)
BOOTOBJS = $(filter-out start.o, $(OBJS))
KERNELOBJS = $(filter-out boot.o, $(OBJS))
INC = $(wildcard lib/)

BOOT = bootloader
BOOTELF = $(BOOT).elf
BOOTIMG = $(BOOT).img

KERNEL = kernel8
KERNELELF = $(KERNEL).elf
KERNELIMG = $(KERNEL).img

CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary
OD = aarch64-linux-gnu-objdump



GDB = aarch64-linux-gnu-gdb
GDBFLAGS = -x ./debug.gdb

all:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC) -c -g $(SRCS)
	make bootloader
	make kernel

bootloader:
	$(LD) $(LDBOOTFLAGS) -o $(BUILD_DIR)/$(BOOTELF) $(BOOTOBJS)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(BOOTELF) $(BUILD_DIR)/$(BOOTIMG)
	$(OD) -D $(BUILD_DIR)/$(BOOTELF) > $(BUILD_DIR)/$(BOOT).objdump
	$(OD) -x $(BUILD_DIR)/$(BOOTELF) > $(BUILD_DIR)/$(BOOT).xobjdump
	$(OD) -s $(BUILD_DIR)/$(BOOTELF) > $(BUILD_DIR)/$(BOOT).content

kernel:
	$(LD) $(LDKERNELFLAGS) -o $(BUILD_DIR)/$(KERNELELF) $(KERNELOBJS)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(KERNELELF) $(BUILD_DIR)/$(KERNELIMG)
	$(OD) -D $(BUILD_DIR)/$(KERNELELF) > $(BUILD_DIR)/$(KERNEL).objdump
	$(OD) -x $(BUILD_DIR)/$(KERNELELF) > $(BUILD_DIR)/$(KERNEL).xobjdump
	$(OD) -s $(BUILD_DIR)/$(KERNELELF) > $(BUILD_DIR)/$(KERNEL).content
	rm $(OBJS)


clean:
	rm -rf $(BUILD_DIR)

debug:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(BOOT).img -display none

gdb:
	$(GDB) $(GDBFLAGS)

run:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(BOOT).img -display none -serial null -serial stdio

test: 
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/$(BOOT).img -serial null -serial pty -initrd initramfs.cpio