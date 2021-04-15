.PHONY: all clean
CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Iinclude -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles

LD = aarch64-linux-gnu-ld
LDFLAGS = -T scripts/linker.ld

OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary

BUILD_DIR = rootfs
ELF = user.elf
IMG = user.img
SRC = $(wildcard *.S) $(wildcard *.c)
OBJ = user.o $(filter-out user.o, $(wildcard *.o))

all:
	make clean
	make user.img
	cd rootfs && find . | cpio -o -H newc > ../initramfs.cpio

user.img:
	$(CXX) $(CXXFLAGS) -c -g $(SRC)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(ELF) $(OBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(ELF) $(BUILD_DIR)/$(IMG)
	rm $(OBJ)
	rm $(BUILD_DIR)/$(ELF)

clean:
	-rm *.cpio