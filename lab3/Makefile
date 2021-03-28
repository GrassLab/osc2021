CROSS_COMPILE ?= aarch64-linux-gnu-

arch := $(shell uname -m)
ifeq ($(arch), arm64)
	undefine CROSS_COMPILE
endif

ifeq ($(arch), aarch64)
	undefine CROSS_COMPILE
endif

CC := $(CROSS_COMPILE)gcc
KERNEL_CFLAGS = -mcpu=cortex-a53 \
	-fno-common -ffreestanding -O0 \
	-fdata-sections -ffunction-sections \
	-Wall -Werror \
	-Wl,-Tkernel/linker.ld,--gc-sections -nostartfiles -Wno-unused-function

BOOTLOADER_CFLAGS = -mcpu=cortex-a53 \
	-fno-common -ffreestanding -O0 \
	-Wall -Werror \
	-Wl,-Tbootloader/linker.ld -nostartfiles -Wno-unused-function

.PHONY: gen_cpio clean


all: kernel8.img bootloader.img gen_cpio

debug: KERNEL_CFLAGS += -DDEBUG
debug: BOOTLOADER_CFLAGS += -DDEBUG
debug: all

# TODO: make it more efficient
kernel8.img: kernel/startup.S kernel/main.c lib/*.c
	$(CC) $(KERNEL_CFLAGS) $^ -o kernel8.elf -I.
	$(CROSS_COMPILE)objcopy -O binary kernel8.elf kernel8.img

bootloader.img: bootloader/startup.S bootloader/main.c lib/*.c
	$(CC) $(BOOTLOADER_CFLAGS) $^ -o bootloader.elf -I.
	$(CROSS_COMPILE)objcopy -O binary bootloader.elf bootloader.img

run: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -display none -serial null -serial pty

gen_cpio:
	cd initramfs && find . -print0 | cpio --null -ov --format=newc > ../initramfs.cpio

clean:
	rm -f *.elf *.img *.o initramfs.cpio
