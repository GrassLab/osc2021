CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -ggdb -Og -nostdlib -nostartfiles -ffreestanding -no-pie -Iinclude -Ilib -Iprogram -Iasm
LD = aarch64-linux-gnu-ld
LDFLAGS = -T program/linker.ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary
BUILD_DIR=.
HEADER := $(wildcard */*.h)
SRC := $(wildcard */*.c)
ASM := $(wildcard */*.S) $(wildcard */*/*.S)
ELF = virtual_test
OBJ := $(patsubst %.S, %.o, $(ASM)) $(patsubst %.c, %.o, $(SRC))
.PHONY: all clean rootfs 

all: $(ELF) 
%o: %s %c	
$(ELF): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(wildcard */*.o) $(wildcard */*/*.o)
	rm -f $(ELF)
rootfs:
	cp $(ELF) ../rootfs/$(ELF)
	cd ../rootfs; find . | cpio -o -H newc > ../initramfs.cpio




