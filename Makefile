TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy

SRC_DIR = src
DST_DIR = img

LINKER_FILE = $(SRC_DIR)/linker.ld
ENTRY = $(SRC_DIR)/boot.S
ENTRY_OBJ = $(SRC_DIR)/boot.o

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(SRCS:.c=.o)

CFLAGS = -I include/ -Wall -fno-builtin-memset

.PHONY: clean all asm run

all: $(DST_DIR)/kernel8.img

$(ENTRY_OBJ): $(ENTRY)
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DST_DIR)/kernel8.img: $(ENTRY_OBJ) $(OBJS)
	$(LD) -T $(LINKER_FILE) -o $(DST_DIR)/kernel8.elf $^
	$(OBJCOPY) -O binary $(DST_DIR)/kernel8.elf $(DST_DIR)/kernel8.img


asm: $(DST_DIR)/kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel $(DST_DIR)/kernel8.img -display none -d in_asm

run: $(DST_DIR)/kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel $(DST_DIR)/kernel8.img -display none -serial null -serial stdio

clean:
	rm -rf $(SRC_DIR)/*.o $(DST_DIR)/kernel8.*
