# define variable
# -g for gdb debug
# -Wall is show warning message
# -fno-builtin is Don’t recognize built-in functions, ex. strlen, strcmp
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
CFLAGS = -g -Wall -fno-builtin
LD_FLAGS = -T linker.ld

# 'all' keyword represent this makefile target, so target is generate bootloader.img
all: bootloader.img

# compile startup.s, generate startup.o
startup.o: startup.s
	$(CC) -c $< -o $@

# compile all *.c and generate *.o
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate bootloader.img, need startup.o main.o util.o uart.o reboot.o linker.ld 
bootloader.img: startup.o main.o util.o uart.o reboot.o kernel.o linker.ld
	$(LD) $(LD_FLAGS) -o bootloader.elf startup.o main.o util.o uart.o reboot.o kernel.o
	$(OBJCOPY) -O binary bootloader.elf $@

# clean all generate file
clean:
	rm *.o bootloader.elf bootloader.img

# check asm by qemu
asm: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -d in_asm

# run bootloader.img for test
run: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial stdio

# pseudo TTY
tty: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial pty

# gdb debug
debug: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -S -s
