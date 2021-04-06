CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
CP = aarch64-linux-gnu-objcopy
SRCS = $(shell find . -name '*.c')
OBJS = $(SRCS:.c=.o)

all: kernel8.img

%.o: %.c
	$(CC) -c -g -Wall -I . -ffreestanding -o $@ $<

boot/start.o: boot/start.s
	$(CC) -c -g -Wall -o $@ $<

kernel8.elf: boot/boot.ld boot/start.o $(OBJS)
	$(LD) -nostdlib -T boot/boot.ld -o kernel8.elf boot/start.o $(OBJS)

kernel8.img: kernel8.elf
	$(CP) -O binary kernel8.elf kernel8.img

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio
