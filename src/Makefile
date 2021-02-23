# this makefile should be runned under linux cross compile env
ARCH_PRE := aarch64-unknown-linux-gnueabi

CC := $(ARCH_PRE)-gcc
LD:= $(ARCH_PRE)-ld
OBJCOPY := $(ARCH_PRE)-objcopy
KERNEL := kernel8

ALL: $(KERNEL).img
GENERATED := a.o $(KERNEL).elf $(KERNEL).img

$(KERNEL).img: $(KERNEL).elf
	$(OBJCOPY) -O binary $(KERNEL).elf $@

$(KERNEL).elf: a.o linker.ld
	$(LD) -T linker.ld -o $@ a.o

a.o: a.S
	$(CC) -c $^

.PHONY: clean
clean:
	rm $(GENERATED)