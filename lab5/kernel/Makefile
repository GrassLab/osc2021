# define variable
# -g for gdb debug
# -Wall is show warning message
# -fno-builtin is Don’t recognize built-in functions, ex. strlen, strcmp
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
CFLAGS = -g -Wall -fno-builtin
LD_FLAGS = -T linker.ld

# 'all' keyword represent this makefile target, so target is generate kernel8.img
all: kernel8.img user_program argv_test fork_test gencpio

%.o : %.s
	$(CC) -c $< -o $@

# compile all *.c and generate *.o
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate kernel8.img, need startup.o main.o util.o uart.o reboot.o cpio.o linker.ld 
kernel8.img: startup.o main.o util.o uart.o reboot.o cpio.o devicetree.o buddy.o allocator.o exception.o exception_handle.o timer.o core_timer.o context_switch.o thread.o linker.ld
	$(LD) $(LD_FLAGS) -o kernel8.elf startup.o main.o util.o uart.o reboot.o cpio.o devicetree.o buddy.o allocator.o exception.o exception_handle.o timer.o core_timer.o context_switch.o thread.o
	$(OBJCOPY) -O binary kernel8.elf $@

# generate boot.o
../user_program/svc.o : ../user_program/svc.s
	$(CC) $(CFLAGS) -c $< -o $@

# generate boot.elf and put rootfs	
user_program : ../user_program/svc.o
	aarch64-linux-gnu-ld -o ../rootfs/svc.elf ../user_program/svc.o

../user_program/argv_test/%.o : ../user_program/argv_test/%.s
	$(CC) -c $< -o $@

../user_program/argv_test/%.o : ../user_program/argv_test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

argv_test : ../user_program/argv_test/startup.o ../user_program/argv_test/sys.o ../user_program/argv_test/argv_test.o ../user_program/argv_test/uart.o  ../user_program/argv_test/util.o 
	$(LD) -T ../user_program/argv_test/linker.ld -o ../user_program/argv_test/argv_test.elf ../user_program/argv_test/startup.o ../user_program/argv_test/sys.o ../user_program/argv_test/argv_test.o ../user_program/argv_test/uart.o ../user_program/argv_test/util.o 
	$(OBJCOPY) -O binary ../user_program/argv_test/argv_test.elf ../rootfs/argv_test.img
	
../user_program/fork_test/%.o : ../user_program/fork_test/%.s
	$(CC) -c $< -o $@

../user_program/fork_test/%.o : ../user_program/fork_test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

fork_test : ../user_program/fork_test/startup.o ../user_program/fork_test/sys.o ../user_program/fork_test/fork_test.o ../user_program/fork_test/uart.o  ../user_program/fork_test/util.o 
	$(LD) -T ../user_program/fork_test/linker.ld -o ../user_program/fork_test/fork_test.elf ../user_program/fork_test/startup.o ../user_program/fork_test/sys.o ../user_program/fork_test/fork_test.o ../user_program/fork_test/uart.o ../user_program/fork_test/util.o 
	$(OBJCOPY) -O binary ../user_program/fork_test/fork_test.elf ../rootfs/fork_test.img

gencpio:
	cd ../rootfs; find . | cpio -o -H newc > ../initramfs.cpio

# clean all generate file
clean:
	rm *.o kernel8.elf kernel8.img ../initramfs.cpio ../user_program/svc.o ../rootfs/svc.elf ../user_program/argv_test/*.o ../user_program/argv_test/*.elf ../rootfs/argv_test.img ../user_program/fork_test/*.o ../user_program/fork_test/*.elf ../rootfs/fork_test.img

# check asm by qemu
asm: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

# run kernel8.img for test
run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd ../initramfs.cpio -dtb ../bcm2710-rpi-3-b-plus.dtb

# gdb debug
debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
