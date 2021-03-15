SRCS = $(wildcard ./src/*.c)
OBJS = $(addprefix ./build/, $(notdir $(SRCS:.c=.o)))

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = build

CFLAGS = -Wall -O2 -ffreestanding -nostdlib
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy

.PHONY: clean run deploy

all: kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(LINKER) -nostdlib start.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf kernel8.img start.o build/*.o >/dev/null 2>/dev/null || true

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

deploy:
	cp ./kernel8.img /run/media/brothre23/4DFF-0A36/
	sudo eject /dev/sdc