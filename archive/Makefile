.PHONY: all clean
CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Iinclude -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles

LD = aarch64-linux-gnu-ld
LDFLAGS = -T scripts/linker.ld

OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary

BUILD_DIR = build
ELF = user.elf
IMG = user.img
SRC = $(wildcard *.S) $(wildcard *.c)
OBJ = user.o $(filter-out user.o, $(wildcard *.o))

all:
	make clean
	cd rootfs && find . | cpio -o -H newc > ../initramfs.cpio

clean:
	-rm *.cpio