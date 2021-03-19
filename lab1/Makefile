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
all: kernel8.img

# compile startup.s, generate startup.o
startup.o: startup.s
	$(CC) -c $< -o $@

# compile all *.c and generate *.o
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate kernel8.img, need startup.o main.o mystd.o uart.o reboot.o linker.ld 
kernel8.img: startup.o main.o mystd.o uart.o reboot.o linker.ld
	$(LD) $(LD_FLAGS) -o kernel8.elf startup.o main.o mystd.o uart.o reboot.o
	$(OBJCOPY) -O binary kernel8.elf $@

# clean all generate file
clean:
	rm *.o kernel8.elf kernel8.img

# check asm by qemu
asm: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

# run kernel8.img for test
run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

# gdb debug
debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
