SRCS := $(wildcard *.c)
SRCS += $(wildcard *.S)
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)
TARGET = kernel8.img
ELF_TARGET = kernel8.elf
CPIO = initramfs.cpio
DTB = bcm2710-rpi-3-b-plus.dtb

CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OC = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

CFLAGS = -g -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib

.PHONY: all clean run debug 

all: clean $(TARGET)

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -T linker.ld -o $(ELF_TARGET)
	$(OC) -O binary $(ELF_TARGET) $(TARGET)

clean:
	- rm *.o $(ELF_TARGET) > /dev/null 2> /dev/null

run:
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial null -display none -serial stdio -initrd $(CPIO) -dtb $(DTB)

debug:
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial null -display none -serial stdio -initrd $(CPIO) -dtb $(DTB) -S -s -d int
