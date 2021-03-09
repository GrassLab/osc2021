CFLAGS= -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
INCLUDE= includes
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


bootloader.img: bootloader.elf
	$(GNU_OBJECT) -O binary bootloader.elf bootloader.img

bootloader.elf: start.o uart.o bootloader.o linker.ld utils.o
	$(GNU_LD) -T linker.ld -o bootloader.elf start.o bootloader.o uart.o utils.o

start.o: start.S
	$(GNU_GCC) $(CFLAGS) -c start.S

bootloader.o: bootloader.c
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c bootloader.c

uart.o: $(INCLUDE)/uart.h $(INCLUDE)/uart.c $(INCLUDE)/gpio.h
	$(GNU_GCC) $(CFLAGS) -c $(INCLUDE)/uart.c

utils.o: utils.h utils.S
	$(GNU_GCC) $(CFLAGS) -c  utils.S 

clean:
	$(DEL) *.o bootloader.elf bootloader.img

d:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -d in_asm 

r:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial stdio -display none 

p:
	qemu-system-aarch64 -M raspi3 -serial null -serial pty

debug:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -S -s 
