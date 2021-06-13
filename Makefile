TOOLCHAIN_PREFIX = aarch64-linux-gnu-
CC = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld
OBJCPY = $(TOOLCHAIN_PREFIX)objcopy

SRC_DIR = src
OUT_DIR = out

LINKER_FILE = $(SRC_DIR)/linker.ld
ASM_SRCS = $(wildcard $(SRC_DIR)/*.S)
ASM_OBJS = $(ASM_SRCS:$(SRC_DIR)/%.S=$(OUT_DIR)/%.o)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OUT_DIR)/%.o)
IMG_FILE = sfn_nctuos.img

CFLAGS = -Wall -I include -c

.PHONY: all clean asm run debug directories

all: directories kernel8.img

$(OUT_DIR)/%.o: $(SRC_DIR)/%.S
	$(CC) $(CFLAGS) $< -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

kernel8.img: $(OBJS) $(ASM_OBJS)
	$(LD) $(ASM_OBJS) $(OBJS) -T $(LINKER_FILE) -o kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img

asm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio -drive if=sd,file=$(IMG_FILE),format=raw 

debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s -initrd initramfs.cpio

tty: all
	sudo python send_img.py /dev/ttyUSB0 kernel8.img

directories: $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

clean:
	rm -f out/* kernel8.*
