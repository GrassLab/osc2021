CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Iinclude -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles

LD = aarch64-linux-gnu-ld
LDFLAGS = -T scripts/linker.ld

OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary

GDB = gdb-multiarch
GDBFLAGS = -x ./debug.gdb

BUILD_DIR = build
ELF = kernel8.elf
IMG = kernel8.img
SRC = $(wildcard kernel/*.S) $(wildcard kernel/*.c)
OBJ = start.o $(filter-out start.o, $(wildcard *.o))


all:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -g $(SRC)
	make kernel8

kernel8:
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(ELF) $(OBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(ELF) $(BUILD_DIR)/$(IMG)
	rm $(OBJ)

debug:
	qemu-system-aarch64 -M raspi3\
		-kernel $(BUILD_DIR)/$(IMG)\
		-display none\
		-serial null\
		-serial stdio\
		-S -s\
		-initrd archive/initramfs.cpio\

debug_cpio:
	qemu-system-aarch64 -M raspi3\
		-kernel $(BUILD_DIR)/$(IMG)\
		-display none\
		-serial null\
		-serial stdio\
		-S -s\
		-initrd archive/initramfs.cpio
run:
	qemu-system-aarch64 -M raspi3\
		-kernel $(BUILD_DIR)/$(IMG)\
		-display none\
		-serial null\
		-serial stdio\
		-initrd archive/initramfs.cpio\
		

gdb:
	$(GDB) $(GDBFLAGS)

clean:
	rm -rf $(BUILD_DIR)

