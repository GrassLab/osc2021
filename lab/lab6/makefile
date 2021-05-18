CFLAGS= -fPIC -Iinclude -fno-stack-protector -nostdlib -nostartfiles -ffunction-sections -fno-builtin
INCLUDE= includes
G=
# -g
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
	CPIO=cd ./rootfs &&	find . | cpio -o -H newc > ../initramfs.cpio && cd ..
endif

ifeq ($(v), -1)
	CONNECT=sudo screen /dev/ttyUSB0 115200
endif

bootloader.img: clean bootloader.elf kernel.img  ./rootfs/fork_test.img ./rootfs/argv_test.img ./rootfs/tmpfs_test.img ./rootfs/ls.img
	$(GNU_OBJECT) -O binary bootloader.elf bootloader.img
bootloader.elf: bstart.o uart.o bootloader.o linker.ld utils.o 
	$(GNU_LD) -T linker.ld -o bootloader.elf bstart.o bootloader.o uart.o utils.o
bstart.o: bstart.S
	$(GNU_GCC) $(CFLAGS) -c $(G) bstart.S
bootloader.o: bootloader.c
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c $(G) bootloader.c

kernel.img: kernel.elf
	$(GNU_OBJECT) -O binary kernel.elf kernel.img
kernel.elf: kstart.o uart.o kernel.o linker.ld utils.o printf.o
	$(GNU_LD) -T linker.ld -o kernel.elf kstart.o kernel.o uart.o utils.o printf.o
kstart.o: kstart.S
	$(GNU_GCC) $(CFLAGS) -c $(G) kstart.S
kernel.o: kernel.c kernel.h
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c $(G) kernel.c


./rootfs/tmpfs_test.img: tmpfs_test.elf
	$(GNU_OBJECT) -O binary tmpfs_test.elf ./rootfs/tmpfs_test.img
	$(CPIO)
tmpfs_test.elf: tmpfs_test.o start.o user_C.o user_S.o tlinker.ld
	$(GNU_LD) -o tmpfs_test.elf tmpfs_test.o start.o user_C.o user_S.o -T tlinker.ld
start.o: start.S
	$(GNU_GCC) $(CFLAGS) -c $(G) start.S
tmpfs_test.o: tmpfs_test.c
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c $(G) tmpfs_test.c


./rootfs/ls.img: ls.elf
	$(GNU_OBJECT) -O binary ls.elf ./rootfs/ls.img
	$(CPIO)
ls.elf: ls.o start.o user_C.o user_S.o tlinker.ld
	$(GNU_LD) -o ls.elf ls.o start.o user_C.o user_S.o -T tlinker.ld
start.o: start.S
	$(GNU_GCC) $(CFLAGS) -c $(G) start.S
ls.o: ls.c
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c $(G) ls.c

./rootfs/argv_test.img: argv_test.elf
	$(GNU_OBJECT) -O binary argv_test.elf ./rootfs/argv_test.img
	$(CPIO)
argv_test.elf: argv_test.o start.o user_C.o user_S.o tlinker.ld
	$(GNU_LD) -o argv_test.elf argv_test.o start.o user_C.o user_S.o -T tlinker.ld
start.o: start.S
	$(GNU_GCC) $(CFLAGS) -c $(G) start.S
argv_test.o: argv_test.c
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c $(G) argv_test.c

./rootfs/fork_test.img: fork_test.elf
	$(GNU_OBJECT) -O binary fork_test.elf ./rootfs/fork_test.img
	$(CPIO)
fork_test.elf: fork_test.o start.o user_C.o user_S.o tlinker.ld
	$(GNU_LD) -o fork_test.elf fork_test.o start.o user_C.o user_S.o -T tlinker.ld
start.o: start.S
	$(GNU_GCC) $(CFLAGS) -c $(G) start.S
fork_test.o: fork_test.c
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c $(G) fork_test.c

user_C.o:  $(INCLUDE)/user.h $(INCLUDE)/user_C.c
	$(GNU_GCC) $(CFLAGS) -c $(G)  $(INCLUDE)/user_C.c
user_S.o:  $(INCLUDE)/user.h $(INCLUDE)/user_S.S
	$(GNU_GCC) $(CFLAGS) -c $(G) $(INCLUDE)/user_S.S
uart.o: $(INCLUDE)/uart.h $(INCLUDE)/uart.c $(INCLUDE)/gpio.h
	$(GNU_GCC) $(CFLAGS) -c $(G) $(INCLUDE)/uart.c
utils.o: $(INCLUDE)/utils.h $(INCLUDE)/utils.S
	$(GNU_GCC) $(CFLAGS) -c  $(G) $(INCLUDE)/utils.S 
printf.o: $(INCLUDE)/printf.c $(INCLUDE)/printf.h
	$(GNU_GCC) $(CFLAGS) -I $(INCLUDE) -c $(G) $(INCLUDE)/printf.c

clean:
	$(DEL) *.o *.elf *.img

d:
	qemu-system-aarch64 -M raspi3 -kernel kernel.img -display none -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb -S -s

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