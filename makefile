# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

GCC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64
CFLAGS = -g -Wall -fno-builtin
# -g for gdb debugging, -Wall for showing warning message, -fno-builtin for don't recognize built-in functions
# $@: target file, $<: first dependent file, $^: all dependent files
LDFLAGS = -T linker.ld

all: kernel8.img

%.o: %.c  # % => '*' in makefile rules, 
	$(GCC) $(CFLAGS) -c $< -o $@

start.o: start.S
	$(GCC) -c $< -o $@

kernel8.img: start.o main.o uart.o linker.ld
	$(LD) $(LDFLAGS) -o kernel8.elf start.o main.o uart.o
	$(OBJCOPY) -O binary kernel8.elf $@

clean:
	rm *.o kernel8.elf kernel8.img

asm:
	$(QEMU) -M raspi3 -kernel kernel8.img -display none -d in_asm

test: all
	$(QEMU) -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

debug: all
	$(QEMU) -M raspi3 -kernel kernel8.img -display none -s -S