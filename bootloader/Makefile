SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
CC = aarch64-linux-gnu-gcc-8
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy

all: clean bootloader.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bootloader.img: start.o $(OBJS)
	$(LINKER) -nostdlib -nostartfiles start.o $(OBJS) -T linker.ld -o bootloader.elf
	$(OBJ_CPY) -O binary bootloader.elf bootloader.img

clean:
	rm bootloader.elf bootloader.img *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial stdio

