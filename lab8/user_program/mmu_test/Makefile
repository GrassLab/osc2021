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
all: fork_test.img

%.o : %.s
	$(CC) -c $< -o $@

# compile all *.c and generate *.o
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate kernel8.img, need startup.o main.o util.o uart.o reboot.o cpio.o linker.ld 
fork_test.img: startup.o fork_test.o util.o uart.o linker.ld
	$(LD) $(LD_FLAGS) -o fork_test.elf startup.o fork_test.o util.o uart.o
	$(OBJCOPY) -O binary fork_test.elf ../../rootfs/fork_test.img

# clean all generate file
clean:
	rm *.o fork_test.elf ../../rootfs/fork_test.img 
