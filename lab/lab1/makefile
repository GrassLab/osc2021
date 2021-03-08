CFLAGS= -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

ifeq ($(OS), Windows_NT)
	GNU_OBJECT=aarch64-none-linux-gnu-objcopy
	GNU_LD=aarch64-none-linux-gnu-ld
	GNU_GCC=aarch64-none-linux-gnu-gcc
	DEL=del
else
	GNU_OBJECT=aarch64-linux-gnu-objcopy
	GNU_LD=aarch64-linux-gnu-ld
	GNU_GCC=aarch64-linux-gnu-gcc
	DEL=rm -f
endif



kernel8.img: kernel8.elf
	$(GNU_OBJECT) -O binary kernel8.elf kernel8.img

kernel8.elf: start.o uart.o main.o linker.ld 
	$(GNU_LD) -T linker.ld -o kernel8.elf start.o main.o uart.o

start.o: start.S
	$(GNU_GCC) $(CFLAGS) -c start.S

main.o: main.c
	$(GNU_GCC) $(CFLAGS) -c main.c

uart.o: ./uart_gpio/uart.h ./uart_gpio/uart.c ./uart_gpio/gpio.h
	$(GNU_GCC) $(CFLAGS) -c ./uart_gpio/uart.c

clean:
	$(DEL) start.o kernel8.elf kernel8.img

d:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

r:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s 
