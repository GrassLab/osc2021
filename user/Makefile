CC = aarch64-linux-gnu-gcc
CFLAGS = -Wall -g -ffreestanding -march=armv8-a
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary

BUILD_DIR = build
SRC_DIR = src
C_FILES = $(wildcard $(SRC_DIR)/*.c)
TARGET_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%)
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

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.o $(BUILD_DIR)/start.asmo
	$(LD) -T $(LINKER) --gc-sections -g -o $@ $< $(BUILD_DIR)/start.asmo

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/start.asmo: $(SRC_DIR)/start.S
	$(CC) $(CFLAGS) -c $< -o $@
