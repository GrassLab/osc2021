SRCS = $(wildcard ./src/*.c)
OBJS := $(addprefix ./build/, $(notdir $(SRCS:.c=.o)))
OBJS += ./build/start.o

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = build

CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy

.PHONY: clean run deploy

all: bootloader.img

$(OBJ_DIR)/start.o: $(SRC_DIR)/start.S
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

bootloader.img: $(OBJS)
	$(LINKER) -nostdlib -nostartfiles $(OBJS) -T link.ld -o bootloader.elf
	$(OBJ_CPY) -O binary bootloader.elf bootloader.img

clean:
	rm kernel8.elf kernel8.img start.o build/*.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null