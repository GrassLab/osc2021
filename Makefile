SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy

all: clean kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(LINKER) -nostdlib -nostartfiles start.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf kernel8.img *.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio