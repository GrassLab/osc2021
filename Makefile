CC = aarch64-linux-gnu-gcc
CFLAG = -fPIC -Iinclude -fno-stack-protector -nostdlib -nostartfiles -ffunction-sections
SF =
ifndef SF
	SF=start.S
endif
SRC_DIR = src/
SRCS = $(wildcard $(SRC_DIR)*.c)
OBJS = $(SRCS:.c=.o)
all:clean kernel8.img

start.o: $(SRC_DIR)start.S
	$(CC) $(CFLAG) -c $< -o $@
%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAG) -c $< -o $@

kernel8.img: $(SRC_DIR)start.o $(OBJS)
	aarch64-linux-gnu-ld $(SRC_DIR)start.o $(OBJS) -T $(SRC_DIR)linker.ld -nostdlib -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
	
clean:
	-rm $(SRC_DIR)*.o
	-rm kernel8.elf kernel8.img

test:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio -display none

test_cpio:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio -display none -initrd ~/initramfs.cpio

run_serial:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial pty -display none

check:
	aarch64-linux-gnu-readelf -s kernel8.elf