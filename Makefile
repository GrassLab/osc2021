TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)g++
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
DEBUG_DIR = debug
SRC_DIR = src
APP_DIR = app
BOOTLOADER_DIR = bootloader

SD_IMG_PATH = ./sd.img

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
OBJS_Q = $(SRCS:$(SRC_DIR)/%.c=$(DEBUG_DIR)/%.o)
SRCS_ASM = $(wildcard $(SRC_DIR)/*.S)
OBJS_ASM = $(SRCS_ASM:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)
OBJS_ASM_Q = $(SRCS_ASM:$(SRC_DIR)/%.S=$(DEBUG_DIR)/%.o)
CFLAGS = -Wall -O1 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude 

.PHONY: all clean run asm bootloader build_dir debug_dir

all: clean kernel8.img bootloader initramfs

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img: $(OBJS) $(OBJS_ASM)
	$(LD) -nostdlib -nostartfiles $(OBJS) $(OBJS_ASM) -T link.ld -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.S $(DEBUG_DIR)
	$(CC) $(CFLAGS) -D __QEMU__ -c $< -o $@

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c $(DEBUG_DIR)
	$(CC) $(CFLAGS) -D __QEMU__ -c $< -o $@

kernel8_qemu.img: $(OBJS_Q) $(OBJS_ASM_Q)
	$(LD) -nostdlib -nostartfiles $(OBJS_Q) $(OBJS_ASM_Q) -T link.ld -o kernel8_qemu.elf
	$(OBJCPY) -O binary kernel8_qemu.elf kernel8_qemu.img

clean:
	rm -f kernel8.elf kernel8_qemu.elf $(BUILD_DIR)/*.o *.cpio $(DEBUG_DIR)/*.o

run: kernel8_qemu.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8_qemu.img -serial null -serial stdio -initrd initramfs.cpio -drive if=sd,file=$(SD_IMG_PATH),format=raw

asm: kernel8_qemu.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8_qemu.img -display none -d in_asm -serial null -serial pty -initrd initramfs.cpio -drive if=sd,file=$(SD_IMG_PATH),format=raw

pty: kernel8_qemu.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8_qemu.img -serial null -serial pty -initrd initramfs.cpio -drive if=sd,file=$(SD_IMG_PATH),format=raw

initramfs:
	$(MAKE) -C $(APP_DIR) all
	cd rootfs; \
	find . | cpio -o -H newc > ../initramfs.cpio

bootloader:
	$(MAKE) -C $(BOOTLOADER_DIR) all

build_dir: $(BUILD_DIR)

debug_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	
$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)
