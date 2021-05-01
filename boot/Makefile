TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

SRC_DIR = src
OUT_DIR = out

LINKER_FILE = $(SRC_DIR)/linker.ld
ENTRY = $(SRC_DIR)/boot.s
ENTRY_OBJS = $(OUT_DIR)/boot.o
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)

CFLAGS = -Wall -I include -c

.PHONY: all clean asm run debug directories

all: directories bootloader.img

$(ENTRY_OBJS): $(ENTRY)
	$(CC) $(CFLAGS) $< -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

bootloader.img: $(OBJS) $(ENTRY_OBJS)
	$(LD) $(ENTRY_OBJS) $(OBJS) -T $(LINKER_FILE) -o bootloader.elf
	$(OBJCPY) -O binary bootloader.elf bootloader.img

asm:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -d in_asm

run: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial stdio

debug: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -S -gdb tcp::1234

serial: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial pty -initrd initramfs.cpio

directories: $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -f out/* bootloader.*
