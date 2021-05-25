CC = aarch64-linux-gnu-gcc
CFLAGS = -Ilib -Wall -ffreestanding -nostdlib -nostartfiles
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary

BUILD_DIR = build
SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

LIB_DIR = lib
LIBS:=$(wildcard $(LIB_DIR)/*.c)
LIB_OBJS = $(LIBS:$(LIB_DIR)/%.c=$(BUILD_DIR)/%_lib.o)

TARGET_FILES = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%)
LINKER = $(SRC_DIR)/linker.ld

all: $(TARGET_FILES)

clean:
	rm $(BUILD_DIR)/*

$(BUILD_DIR)/%: $(BUILD_DIR)/%.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@
	cp $@ ../kernel/rootfs && \
	cd ../kernel/rootfs && \
	rm -f ../initramfs.cpio && \
	find . | cpio -o -H newc > ../initramfs.cpio

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.o $(BUILD_DIR)/start.o $(LIB_OBJS)
	$(LD) -T $(LINKER) -o $@ $< $(BUILD_DIR)/start.o $(LIB_OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%_lib.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/start.o: $(SRC_DIR)/start.S
	$(CC) $(CFLAGS) -c $< -o $@
