SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = bootloader.img
ELF_TARGET = bootloader.elf
CPIO = ../initramfs.cpio

CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OC = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

CFLAGS = -g -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib

.PHONY: all clean run debug 

all: clean $(TARGET)

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): start.o $(OBJS)
	$(LD) $(LDFLAGS) start.o $(OBJS) -T linker.ld -o $(ELF_TARGET)
	$(OC) -O binary $(ELF_TARGET) $(TARGET)

clean:
	- rm *.o $(ELF_TARGET) $(TARGET) > /dev/null 2> /dev/null

run:
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial null -display none -serial pty -initrd $(CPIO)

debug:
	$(QEMU) -M raspi3 -kernel $(TARGET) -display none -serial null -serial pty -S -s -initrd $(CPIO)
