CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -ggdb -Og -nostdlib -nostartfiles -ffreestanding -no-pie -Iinclude -Ilib -Iprogram
LD = aarch64-linux-gnu-ld
LDFLAGS = -T program/linker.ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary
BUILD_DIR=.
HEADER := $(wildcard */*.h)
SRC := $(wildcard */*.c)
ASM := $(wildcard */*/*.S)
ELF = user_test.elf
IMG = user_test
OBJ := program/main.o $(patsubst %.S, %.o, $(ASM)) $(filter-out program/main.o, $(patsubst %.c, %.o, $(SRC)))
.PHONY: all clean rootfs 

all: $(IMG) 
%o: %s %c	
$(ELF): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)
$(IMG): $(ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(wildcard */*.o) $(wildcard */*/*.o)
	rm -f $(ELF)
	rm -f $(IMG)
rootfs:
	cp $(IMG) ../rootfs/$(IMG)
	cp $(ELF) ../rootfs/$(ELF)
	cd ../rootfs; find . | cpio -o -H newc > ../initramfs.cpio




