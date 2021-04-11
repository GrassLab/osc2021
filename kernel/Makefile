CC = aarch64-linux-gnu-gcc
CFLAGS = -Iinclude -Wall -nostdlib -nostartfiles -ffreestanding
# ASMFLAGS = 
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary

QEMU = qemu-system-aarch64

BUILD_DIR = build
SRC_DIR = src

all: kernel8.img

clean:
	rm $(BUILD_DIR)/*

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(CC) $(ASMFLAGS) -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

kernel8.elf: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(LD) -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)

kernel8.img: kernel8.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/kernel8.img

run:
	$(QEMU) -M raspi3 -kernel $(BUILD_DIR)/kernel8.img -display none -serial null -serial stdio \
		-initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb