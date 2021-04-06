CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -ggdb -Og -nostdlib -nostartfiles -ffreestanding -Iinclude -Ilib -Iperipheral -Ikernel -Irootfs -Imm
LD = aarch64-linux-gnu-ld
LDFLAGS = -T kernel/linker.ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary
BUILD_DIR=.
HEADER := $(wildcard */*.h)
SRC := $(wildcard */*.c)
ASM := $(wildcard */*/*.S)
ELF = $(BUILD_DIR)/kernel.elf
OBJ := $(patsubst %.S, %.o, $(ASM)) $(patsubst %.c, %.o, $(SRC))
IMG = $(BUILD_DIR)/kernel8.img
.PHONY: all clean run dump debug

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

run:
	qemu-system-aarch64 -M raspi3\
		-kernel $(IMG)\
		-display none\
		-serial null\
		-serial stdio\
		-serial pty\
		-S -s
dump:
	qemu-system-aarch64 -M raspi3\
	       	-kernel $(IMG)\
	       	-display none\
	       	-d in_asm
debug:
	qemu-system-aarch64 -M raspi3\
                -kernel $(IMG)\
		-display none\
                -serial null\
                -serial pty\
		-initrd initramfs.cpio\
		-dtb bcm2710-rpi-3-b-plus.dtb\
		-S -s



