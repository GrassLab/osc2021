SRCS = $(wildcard ./src/*.c)
OBJS := $(addprefix ./build/, $(notdir $(SRCS:.c=.o)))
OBJS += ./build/start.o

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = build

CFLAGS = -Wall -O2 -nostdlib -nostdinc
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy

.PHONY: clean run deploy

all: bootloader.img

$(OBJ_DIR)/start.o: $(SRC_DIR)/start.S
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

bootloader.img: $(OBJS) link.ld
	$(LINKER) -nostdlib $(OBJS) -T link.ld -o bootloader.elf
	$(OBJ_CPY) -O binary bootloader.elf bootloader.img

clean:
	rm bootloader.elf bootloader.img build/*.o >/dev/null 2>/dev/null || true

deploy: all
	cp ./bootloader.img /run/media/brothre23/4DFF-0A36/
	sudo eject /dev/sdc

run: all
	# qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial stdio -serial pty
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial stdio -serial pty