KERNEL:=kernel8
LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles
LIB = ./lib
all: $(KERNEL).img

$(KERNEL).img:start.o rd.o main.o lib/uart.o  lib/shell.o lib/stringUtils.o lib/initrd.o
	aarch64-linux-gnu-ld  -T $(LINKER) -o $(KERNEL).elf start.o rd.o main.o lib/uart.o lib/stringUtils.o  lib/shell.o lib/initrd.o
	aarch64-linux-gnu-objcopy -O binary $(KERNEL).elf $@

cpio:
	ls *.md *.c  | cpio -H hpodc -o >ramdisk

start.o:start.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<
rd.o:ramdisk
	aarch64-linux-gnu-ld -r -b binary -o rd.o ramdisk
%.o:%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -o $@ -c $<
$(LIB)/%.o:$(LIB)/%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -o $@ -c $<



run:
	qemu-system-aarch64 -M raspi3 -kernel $(KERNEL).img -display none -serial null -serial stdio

onboard:
	sudo screen /dev/ttyUSB0 115200

clean:
	rm *.o *.elf *.img $(LIB)/*.o 
