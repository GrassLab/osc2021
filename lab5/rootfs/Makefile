SRCS := $(wildcard *.c)
SRCS += $(wildcard *.S)

CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OC = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

CFLAGS = -g -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib

.PHONY: all clean debug

all: clean argv_test fork_test

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

argv_test: argv_test.o systemcall.o utils.o start.o
	$(LD) $(LDFLAGS) -T linker.ld -o argv_test.elf argv_test.o systemcall.o utils.o start.o
	$(OC) -O binary argv_test.elf argv_test

fork_test: fork_test.o systemcall.o utils.o start.o
	$(LD) $(LDFLAGS) -T linker.ld -o fork_test.elf fork_test.o systemcall.o utils.o start.o
	$(OC) -O binary fork_test.elf fork_test

clean:
	- rm *.o $(ELF_TARGET) > /dev/null 2> /dev/null

debug:
	$(QEMU) -M raspi3 -kernel argv_test -serial null -display none -serial stdio -S -s