CC = aarch64-linux-gnu-gcc
CCFLAGS = -Wall -g -nostdlib -nostartfiles -ffreestanding -Iinclude
LD = aarch64-linux-gnu-ld
LDFLAGS = -T linker.ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary
SOURCE_DIR=.
BUILD_DIR=.
SRC = $(wildcard **/*.S) $(wildcard **/*.c)
ELF = kernel.elf
OBJ = start.o $(filter-out start.o, $(wildcard *.o))
IMG = kernel8.img

all:
	$(CC) $(CCFLAGS) -c $(SRC)
	make ld
ld:	
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(ELF) $(OBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(ELF) $(BUILD_DIR)/$(IMG)
clean:
	rm -f $(OBJ)
	rm -f $(BUILD_DIR)/$(ELF)
	rm -f $(BUILD_DIR)/$(IMG)

run:
	qemu-system-aarch64 -M raspi3\
		-kernel $(BUILD_DIR)/$(IMG)\
		-display none\
		-serial null\
		-serial stdio\
		-S -s
dump:
	qemu-system-aarch64 -M raspi3\
	       	-kernel $(BUILD_DIR)/$(IMG)\
	       	-display none\
	       	-d in_asm



