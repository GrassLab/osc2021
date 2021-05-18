SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: start.o main.o uart.o reboot.o kernel8.img

start.o: start.S
	aarch64-linux-gnu-gcc $(CXXFLAGS) -c start.S -o start.o

main.o: main.c
	aarch64-linux-gnu-gcc $(CXXFLAGS) -c main.c -o main.o

uart.o: uart.c
	aarch64-linux-gnu-gcc $(CXXFLAGS) -c uart.c -o uart.o

reboot.o: reboot.c
	aarch64-linux-gnu-gcc $(CXXFLAGS) -c reboot.c -o reboot.o

kernel8.img: start.o $(OBJS)
	aarch64-linux-gnu-ld -nostdlib -nostartfiles start.o $(OBJS) -T linker.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img


clean:
	rm -rf kernel8.elf *.o

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s

run:
	qemu-system-aarch64 -serial null -serial pty -M raspi3 -kernel kernel8.img
	#qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio