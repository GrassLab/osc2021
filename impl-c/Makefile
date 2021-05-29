TOOLCHAIN_PREFIX ?= aarch64-unknown-linux-gnueabi
CC := $(TOOLCHAIN_PREFIX)-gcc
LD:= $(TOOLCHAIN_PREFIX)-ld
OBJCOPY := $(TOOLCHAIN_PREFIX)-objcopy

SRC_DIR = src

LDSCRIPT = kernel/linker.ld
LDFLAGS = -nostdlib

SRC = $(wildcard **/*.S) $(wildcard **/*.c)
OBJS = $(wildcard ./*.o)
BUILD_OBJS = $(wildcard build/*.o)

CFLAGS = -Wall -Iinclude/libs -Iinclude -nostartfiles -ffreestanding
CFLAGS += -g -ggdb

TARGET = kernel8.img
TARGET_ELF = kernel8.elf

.PHONY: all
all:
	$(CC) $(CFLAGS) -c $(SRC)
	mkdir -p build
	mv ./*.o build
	make kernel

.PHONY: kernel
kernel:
	$(LD) $(LDFLAGS)  $(BUILD_OBJS) -T $(LDSCRIPT) -o $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET)

clean:
	$(RM) $(OBJS)
	$(RM) -rf build
	$(RM) $(TARGET) $(TARGET_ELF)


