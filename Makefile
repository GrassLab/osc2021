ARMGNU = aarch64-linux-gnu
C_FLAGS = -Wall -Wextra -Wshadow -Wconversion -ffreestanding -nostdinc -nostdlib -nostartfiles

KERNEL_C_SOURCE_FILES = main.c uart.c
KERNEL_C_OBJECT_FILES = $(KERNEL_C_SOURCE_FILES:.c=.o)

all: kernel8.img

start.o: start.S
		$(ARMGNU)-gcc -c start.S -o start.o -g

%.o: %.c 
		$(ARMGNU)-gcc $(C_FLAGS) -c $< -o $@

kernel8.elf: start.o $(KERNEL_C_OBJECT_FILES) 
		$(ARMGNU)-ld start.o $(KERNEL_C_OBJECT_FILES) -T linker.ld -o kernel8.elf

kerenl8.img: kernel8.elf
		$(ARMGNU)-objcopy -O binary kernel8.elf kernel8.img

clean: rm -rf *.o *.elf *img

run: kernel8.img
		qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

debug: kernel8.img 
		qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
