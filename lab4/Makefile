CROSS_COMPILE ?= aarch64-linux-gnu-

arch := $(shell uname -m)
ifeq ($(arch), arm64)
	undefine CROSS_COMPILE
endif

ifeq ($(arch), aarch64)
	undefine CROSS_COMPILE
endif

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
BUILD_DIR := build
SRC_DIRS := lib

SRCS := $(shell find $(SRC_DIRS) -name *.c)

KERNEL_SRCS := $(SRCS) kernel/startup.S kernel/main.c kernel/exc_handler.c
KERNEL_OBJS := $(KERNEL_SRCS:%=$(BUILD_DIR)/%.o)
KERNEL_DEPS := $(KERNEL_OBJS:.o=.d)

BL_SRCS := lib/uart.c lib/printf.c bootloader/startup.S bootloader/main.c
BL_OBJS := $(BL_SRCS:%=$(BUILD_DIR)/%.o)
BL_DEPS := $(BL_OBJS:.o=.d)

DEPS := $(KERNEL_DEPS) $(BL_DEPS)

CFLAGS := -mcpu=cortex-a53+nofp \
	-fno-common -ffreestanding -O0 \
	-fdata-sections -ffunction-sections \
	-Wall -Werror -nostartfiles -Iinclude -g

.PHONY: gen_cpio clean

all: kernel8.img bootloader.img gen_cpio

debug: KERNEL_CFLAGS += -DDEBUG
debug: BOOTLOADER_CFLAGS += -DDEBUG
debug: all

kernel8.img: $(KERNEL_OBJS)
	$(LD) -Tkernel/linker.ld $^ -o kernel8.elf
	$(CROSS_COMPILE)objcopy -O binary kernel8.elf kernel8.img

bootloader.img: $(BL_OBJS)
	$(LD) -Tbootloader/linker.ld $^ -o bootloader.elf
	$(CROSS_COMPILE)objcopy -O binary bootloader.elf bootloader.img

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: %.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial pty

gen_cpio:
	cd initramfs && find . -print0 | cpio --null -ov --format=newc > ../initramfs.cpio

clean:
	rm -rf build *.elf *.img *.o initramfs.cpio

-include $(DEPS)
