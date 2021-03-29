KERNEL:=kernel8
LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles
LIB = ./lib
CPIO:=initramfs.cpio
all: $(KERNEL).img

$(KERNEL).img:start.o rd.o main.o lib/uart.o  lib/shell.o lib/stringUtils.o lib/initrd.o lib/printf.o
	aarch64-linux-gnu-ld  -T $(LINKER) -o $(KERNEL).elf start.o rd.o main.o lib/uart.o lib/stringUtils.o  lib/shell.o lib/initrd.o lib/printf.o
	aarch64-linux-gnu-objcopy -O binary $(KERNEL).elf $@

cpio:
	cd rootfs;\
	find . | cpio -H newc -o >../$(CPIO);

start.o:start.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<
rd.o:$(CPIO)
	aarch64-linux-gnu-ld -r -b binary -o rd.o $(CPIO)
%.o:%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -o $@ -c $<
$(LIB)/%.o:$(LIB)/%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -o $@ -c $<



run:
#	qemu-system-aarch64 -M raspi3 -kernel $(KERNEL).img -display none -serial null -serial stdio
	qemu-system-aarch64 -M raspi3 -kernel $(KERNEL).img -display none -serial null -serial stdio -initrd ./initramfs.cpio
try: clean cpio all run

onboard:
	sudo screen /dev/ttyUSB0 115200

.SILENT:clean
clean:
	-@ rm -f *.cpio *.o *.elf *.img $(LIB)/*.o 
