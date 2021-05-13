# use pushd from bash
SHELL = /bin/bash
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
KERNEL_SRC_DIRS := lib kernel
USER_SRC_DIRS := user
USER_BIN_SRC_DIRS := initramfs

KERNEL_SRCS := $(shell find $(KERNEL_SRC_DIRS) -name '*.[cS]')
KERNEL_OBJS := $(KERNEL_SRCS:%=$(BUILD_DIR)/%.o)
KERNEL_DEPS := $(KERNEL_OBJS:.o=.d)

BL_SRCS := lib/uart.c lib/printf.c bootloader/startup.S bootloader/main.c
BL_OBJS := $(BL_SRCS:%=$(BUILD_DIR)/%.o)
BL_DEPS := $(BL_OBJS:.o=.d)

USER_SRCS := $(shell find $(USER_SRC_DIRS) -name '*.[cS]')
USER_OBJS := $(USER_SRCS:%=$(BUILD_DIR)/%.o)
USER_DEPS := $(USER_OBJS:.o=.d)

USER_BIN_SRCS := $(shell find $(USER_BIN_SRC_DIRS) -name '*.c')
USER_BIN_OBJS := $(USER_BIN_SRCS:%=$(BUILD_DIR)/%.o) $(USER_BIN_SRCS:%.c=$(BUILD_DIR)/%)
USER_BIN_DEPS := $(USER_BIN_OBJS:.o=.d)

# shit, we need to wrap a $() outside the deps list to not get
# makefile confused that is another makefile (debug with `make --debug=v`)
DEPS := $($(KERNEL_DEPS),$(BL_DEPS),$(USER_DEPS),$(USER_BIN_DEPS))

CFLAGS := -mcpu=cortex-a53+nofp -MMD \
	-fno-common -ffreestanding -O0 \
	-Wno-parentheses \
	-fdata-sections -ffunction-sections \
	-Wall -Werror -nostartfiles -Iinclude -g

USER_CFLAGS := -mcpu=cortex-a53 -MMD \
	-fno-common -ffreestanding -O0 \
	-Wall -Werror -nostartfiles -Iuser_include -g

.PHONY: clean all

all: kernel8.img bootloader.img initramfs.cpio

debug: CFLAGS += -DDEBUG
debug: all

rpi3: CFLAGS += -DRPI3
rpi3: all

kernel8.img: $(KERNEL_OBJS)
	$(LD) -Tkernel/linker.ld $^ -o kernel8.elf
	$(CROSS_COMPILE)objcopy -O binary kernel8.elf kernel8.img

bootloader.img: $(BL_OBJS)
	$(LD) -Tbootloader/linker.ld $^ -o bootloader.elf
	$(CROSS_COMPILE)objcopy -O binary bootloader.elf bootloader.img

initramfs.cpio: $(USER_BIN_OBJS)
	pushd $(BUILD_DIR)/out_initramfs && \
	find . -print0 | cpio --null -o --format=newc > $$(dirs +1)/initramfs.cpio && \
	popd

$(BUILD_DIR)/initramfs/%: $(BUILD_DIR)/initramfs/%.c.o $(USER_OBJS)
	mkdir -p $(dir $@)
	mkdir -p $(dir $(BUILD_DIR)/out_initramfs/$*)
	$(LD) -Tuser/linker.ld $^ -o $@
	cp $@ $(BUILD_DIR)/out_initramfs/$*
	chmod +x $(BUILD_DIR)/out_initramfs/$*

$(BUILD_DIR)/initramfs/%.c.o: initramfs/%.c
	mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/user/%.c.o: user/%.c
	mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/user/%.S.o: user/%.S
	mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: %.S
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial pty

clean:
	rm -rf build *.elf *.img *.o initramfs.cpio

-include $(DEPS)
