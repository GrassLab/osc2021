.PHONY: all clean
CXX = aarch64-linux-gnu-gcc
CXXFLAGS = -Iinclude -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles

LD = aarch64-linux-gnu-ld
LDFLAGS = -T scripts/linker.ld

OBJCOPY = aarch64-linux-gnu-objcopy
OBJDUMP = aarch64-linux-gnu-objdump
OBJCOPYFLAGS = -O binary

BUILD_DIR = build
ELF = argv_test.elf
IMG = argv_test.img
SRC = $(wildcard kernel/*.S) $(wildcard kernel/*.c)
OBJ = $(wildcard *.o)

all:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -g $(SRC)
	make user.img
	$(OBJDUMP) -D $(BUILD_DIR)/$(ELF) > $(BUILD_DIR)/kernel.objdump

user.img:	
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/$(ELF) $(OBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/$(ELF) $(BUILD_DIR)/$(IMG)
	rm $(OBJ)
	cp $(BUILD_DIR)/$(IMG) ../rootfs/$(IMG)

make clean:
	rm $(OBJ)