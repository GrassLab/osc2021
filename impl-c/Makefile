TOOLCHAIN_PREFIX ?= aarch64-unknown-linux-gnueabi
CC := $(TOOLCHAIN_PREFIX)-gcc
LD:= $(TOOLCHAIN_PREFIX)-ld
OBJCOPY := $(TOOLCHAIN_PREFIX)-objcopy

SRC_DIR = src
BUILD_DIR = build

LDSCRIPT = $(SRC_DIR)/linker.ld
LDFLAGS = -nostdlib
ENTRY = $(SRC_DIR)/start.S
ENTRY_OBJS = $(BUILD_DIR)/start.o

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

CFLAGS = -Wall -I include -nostartfiles -ffreestanding -MMD
CFLAGS += -g -ggdb

TARGET = kernel8.img
TARGET_ELF = kernel8.elf

all: directories $(TARGET)

$(ENTRY_OBJS): $(ENTRY)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(LDSCRIPT) include/cfg.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ENTRY_OBJS) $(OBJS)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $(ENTRY_OBJS) $(OBJS) -T $(LDSCRIPT) -o $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET)


directories: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(RM) -rf $(BUILD_DIR)
	$(RM) $(TARGET) $(TARGET_ELF)


