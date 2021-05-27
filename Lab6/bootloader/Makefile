TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

BUILD_DIR = build
SRC_DIR = src

LINKER_FILE = $(SRC_DIR)/linker.ld
ENTRY = $(SRC_DIR)/start.s
ENTRY_OBJS = $(BUILD_DIR)/start.o
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

CFLAGS = -Wall -I include -c -fno-stack-protector -nostdlib

.PHONY: all clean

all: build_dir bootloader.img

# build

$(ENTRY_OBJS): $(ENTRY)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

bootloader.img: $(OBJS) $(ENTRY_OBJS)
	$(LD) $(ENTRY_OBJS) $(OBJS) -T $(LINKER_FILE) -o bootloader.elf
	$(OBJCPY) -O binary bootloader.elf bootloader.img 

# run emulator

run: $(BUILD_DIR) bootloader.img
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial stdio

display: $(BUILD_DIR) bootloader.img
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial stdio

tty: $(BUILD_DIR) bootloader.img
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial pty

asm:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -d in_asm

debug: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -S -s

# utility 

build_dir: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.elf *.img
