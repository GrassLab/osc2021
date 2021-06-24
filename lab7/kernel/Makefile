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
all: kernel8.img user_program argv_test fork_test vfs_test ls_test multilvl_test fat32_test fat32_test2 gencpio

%.o : %.s
	$(CC) -c $< -o $@

# compile all *.c and generate *.o
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate kernel8.img, need startup.o main.o util.o uart.o reboot.o cpio.o linker.ld 
kernel8.img: startup.o main.o util.o uart.o reboot.o cpio.o devicetree.o buddy.o allocator.o exception.o exception_handle.o timer.o core_timer.o context_switch.o thread.o vfs.o tmpfs.o sdhost.o fat32.o linker.ld
	$(LD) $(LD_FLAGS) -o kernel8.elf startup.o main.o util.o uart.o reboot.o cpio.o devicetree.o buddy.o allocator.o exception.o exception_handle.o timer.o core_timer.o context_switch.o thread.o vfs.o sdhost.o fat32.o tmpfs.o
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
	
../user_program/vfs_test/%.o : ../user_program/vfs_test/%.s
	$(CC) -c $< -o $@

../user_program/vfs_test/%.o : ../user_program/vfs_test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

vfs_test : ../user_program/vfs_test/startup.o ../user_program/vfs_test/sys.o ../user_program/vfs_test/vfs_test.o ../user_program/vfs_test/uart.o  ../user_program/vfs_test/util.o 
	$(LD) -T ../user_program/vfs_test/linker.ld -o ../user_program/vfs_test/vfs_test.elf ../user_program/vfs_test/startup.o ../user_program/vfs_test/sys.o ../user_program/vfs_test/vfs_test.o ../user_program/vfs_test/uart.o ../user_program/vfs_test/util.o 
	$(OBJCOPY) -O binary ../user_program/vfs_test/vfs_test.elf ../rootfs/vfs_test.img
	
../user_program/ls_test/%.o : ../user_program/ls_test/%.s
	$(CC) -c $< -o $@

../user_program/ls_test/%.o : ../user_program/ls_test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

ls_test : ../user_program/ls_test/startup.o ../user_program/ls_test/sys.o ../user_program/ls_test/ls_test.o ../user_program/ls_test/uart.o  ../user_program/ls_test/util.o 
	$(LD) -T ../user_program/ls_test/linker.ld -o ../user_program/ls_test/ls_test.elf ../user_program/ls_test/startup.o ../user_program/ls_test/sys.o ../user_program/ls_test/ls_test.o ../user_program/ls_test/uart.o ../user_program/ls_test/util.o 
	$(OBJCOPY) -O binary ../user_program/ls_test/ls_test.elf ../rootfs/ls_test.img
	
../user_program/multilvl_test/%.o : ../user_program/multilvl_test/%.s
	$(CC) -c $< -o $@

../user_program/multilvl_test/%.o : ../user_program/multilvl_test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

multilvl_test : ../user_program/multilvl_test/startup.o ../user_program/multilvl_test/sys.o ../user_program/multilvl_test/multilvl_test.o ../user_program/multilvl_test/uart.o  ../user_program/multilvl_test/util.o 
	$(LD) -T ../user_program/multilvl_test/linker.ld -o ../user_program/multilvl_test/multilvl_test.elf ../user_program/multilvl_test/startup.o ../user_program/multilvl_test/sys.o ../user_program/multilvl_test/multilvl_test.o ../user_program/multilvl_test/uart.o ../user_program/multilvl_test/util.o 
	$(OBJCOPY) -O binary ../user_program/multilvl_test/multilvl_test.elf ../rootfs/multilvl_test.img
	
../user_program/fat32_test/%.o : ../user_program/fat32_test/%.s
	$(CC) -c $< -o $@

../user_program/fat32_test/%.o : ../user_program/fat32_test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

fat32_test : ../user_program/fat32_test/startup.o ../user_program/fat32_test/sys.o ../user_program/fat32_test/fat32_test.o ../user_program/fat32_test/uart.o  ../user_program/fat32_test/util.o 
	$(LD) -T ../user_program/fat32_test/linker.ld -o ../user_program/fat32_test/fat32_test.elf ../user_program/fat32_test/startup.o ../user_program/fat32_test/sys.o ../user_program/fat32_test/fat32_test.o ../user_program/fat32_test/uart.o ../user_program/fat32_test/util.o 
	$(OBJCOPY) -O binary ../user_program/fat32_test/fat32_test.elf ../rootfs/fat32_test.img
	
../user_program/fat32_test2/%.o : ../user_program/fat32_test2/%.s
	$(CC) -c $< -o $@

../user_program/fat32_test2/%.o : ../user_program/fat32_test2/%.c
	$(CC) $(CFLAGS) -c $< -o $@

fat32_test2 : ../user_program/fat32_test2/startup.o ../user_program/fat32_test2/sys.o ../user_program/fat32_test2/fat32_test2.o ../user_program/fat32_test2/uart.o  ../user_program/fat32_test2/util.o 
	$(LD) -T ../user_program/fat32_test2/linker.ld -o ../user_program/fat32_test2/fat32_test2.elf ../user_program/fat32_test2/startup.o ../user_program/fat32_test2/sys.o ../user_program/fat32_test2/fat32_test2.o ../user_program/fat32_test2/uart.o ../user_program/fat32_test2/util.o 
	$(OBJCOPY) -O binary ../user_program/fat32_test2/fat32_test2.elf ../rootfs/fat32_test2.img

gencpio:
	cd ../rootfs; find . | cpio -o -H newc > ../initramfs.cpio

# clean all generate file
clean:
	rm *.o kernel8.elf kernel8.img ../initramfs.cpio ../user_program/svc.o ../rootfs/svc.elf ../user_program/argv_test/*.o ../user_program/argv_test/*.elf ../rootfs/argv_test.img ../user_program/fork_test/*.o ../user_program/fork_test/*.elf ../rootfs/fork_test.img ../user_program/vfs_test/*.o ../user_program/vfs_test/*.elf ../rootfs/vfs_test.img ../user_program/ls_test/*.o ../user_program/ls_test/*.elf ../rootfs/ls_test.img ../user_program/multilvl_test/*.o ../user_program/multilvl_test/*.elf ../rootfs/multilvl_test.img ../user_program/fat32_test/*.o ../user_program/fat32_test/*.elf ../rootfs/fat32_test.img ../user_program/fat32_test2/*.o ../user_program/fat32_test2/*.elf ../rootfs/fat32_test2.img


# check asm by qemu
asm: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

# run kernel8.img for test
run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd ../initramfs.cpio -dtb ../bcm2710-rpi-3-b-plus.dtb -drive if=sd,file=../sfn_nctuos.img,format=raw

# gdb debug
debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
