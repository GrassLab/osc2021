CFLAGS= -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
INCLUDE= includes
ifeq ($(OS), Windows_NT)
	GNU_OBJECT=aarch64-none-linux-gnu-objcopy
	GNU_LD=aarch64-none-linux-gnu-ld
	GNU_GCC=aarch64-none-linux-gnu-gcc
	DEL=del
	SERIAL=COM$(v)
	CONNECT=putty -load "OSDI"
	PYTHON=python
else
	GNU_OBJECT=aarch64-linux-gnu-objcopy
	GNU_LD=aarch64-linux-gnu-ld
	GNU_GCC=aarch64-linux-gnu-gcc
	DEL=rm -f
	SERIAL=pty
	CONNECT=sudo screen /dev/pts/$(v)
	PYTHON=python3
endif

ifeq ($(v), -1)
	CONNECT=sudo screen /dev/ttyUSB0 115200
endif

bootloader.img: clean bootloader.elf kernel.img
	$(GNU_OBJECT) -O binary bootloader.elf bootloader.img

kernel.img: kernel.elf
	$(GNU_OBJECT) -O binary kernel.elf kernel.img

kernel.elf: start.o uart.o kernel.o linker.ld utils.o
	$(GNU_LD) -T linker.ld -o kernel.elf start.o kernel.o uart.o utils.o

bootloader.elf: start.o uart.o bootloader.o linker.ld utils.o
	$(GNU_LD) -T linker.ld -o bootloader.elf start.o bootloader.o uart.o utils.o

start.o: start.S
	$(GNU_GCC) $(CFLAGS) -c start.S

bootloader.o: bootloader.c
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c bootloader.c

kernel.o: kernel.c kernel.h
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c kernel.c

uart.o: $(INCLUDE)/uart.h $(INCLUDE)/uart.c $(INCLUDE)/gpio.h
	$(GNU_GCC) $(CFLAGS) -c $(INCLUDE)/uart.c

utils.o: $(INCLUDE)/utils.h $(INCLUDE)/utils.S
	$(GNU_GCC) $(CFLAGS) -c  $(INCLUDE)/utils.S 


clean:
	$(DEL) *.o *.elf *.img

d:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -d in_asm  -initrd initramfs.cpio 

r:
	qemu-system-aarch64 -M raspi3 -kernel kernel.img -serial null -serial stdio -display none -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb
	# qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial $(SERIAL) -display none -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb
	# qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial $(SERIAL) -display none 
	# qemu-system-aarch64 -M raspi3 -kernel kernel.img -serial null -serial pty -display none 
	# qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial stdio -display none 


p:
	qemu-system-aarch64 -M raspi3 -serial null -serial pty

connect:
	$(CONNECT)

send:
	$(PYTHON) ./send.py $(v)