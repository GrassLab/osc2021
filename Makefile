CC = aarch64-linux-gnu-gcc
CFLAG = -g -fPIC -Iinclude -fno-stack-protector -nostdlib -nostartfiles -ffunction-sections -fno-builtin
SF =
ifndef SF
	SF=start.S
endif
SRC_DIR = src/
SRCS = $(wildcard $(SRC_DIR)*.c)
ASM_FILE = $(wildcard $(SRC_DIR)*.S)
OBJS = $(SRCS:.c=.o)
OBJS += $(ASM_FILE:.S=.o)
all:clean kernel8.img
GDB = -g

# start.o: $(SRC_DIR)start.S
# 	$(CC) $(CFLAG) -c $< -o $@

# entry.o: $(SRC_DIR)entry.S ./include/entry.h
# 	$(CC) $(CFLAG) -c $< -o $@
%.o: %.S 
	$(CC) $(CFLAG) -c $< -o $@
%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAG) -c $< -o $@

kernel8.img: entry.o $(SRC_DIR)start.o $(OBJS) 
	aarch64-linux-gnu-ld $(OBJS) -T $(SRC_DIR)linker.ld -nostdlib -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	-rm $(SRC_DIR)*.o
	-rm kernel8.elf kernel8.img

test:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio -display none -initrd ../initramfs.cpio
gdb:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s -initrd ../initramfs.cpio
test_lab2:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio -display none -dtb ../OSDI/bcm2710-rpi-3-b-plus.dtb

test_qemu_lab2:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial pty -display none -initrd ../initramfs.cpio -dtb ../OSDI/bcm2710-rpi-3-b-plus.dtb

check:
	aarch64-linux-gnu-readelf -s kernel8.elf

user_program:
	$(CC) $(CFLAG) -c user_program.S -o user_program.elf
	aarch64-linux-gnu-objcopy -O binary user_program.elf user_program.img
	
	