SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

all: clean kernel8.img

start_bl.o: start_bl.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

%.o: %.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

kernel8.img: start_bl.o link_bl.ld $(OBJS)
	aarch64-linux-gnu-ld -nostdlib -nostartfiles start_bl.o $(OBJS) -T link_bl.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf *.o

run: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

asm: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm -serial null -serial pty

pty: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial pty -initrd ../lab2/initramfs.cpio
