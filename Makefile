<<<<<<< HEAD
SRCS = $(wildcard ./src/*.c)
OBJS = $(addprefix ./build/, $(notdir $(SRCS:.c=.o)))

INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = build

CFLAGS = -Wall -O2 -ffreestanding -nostdlib
CC = aarch64-linux-gnu-gcc
LINKER = aarch64-linux-gnu-ld
OBJ_CPY = aarch64-linux-gnu-objcopy
=======
C_SRCS 		= $(wildcard ./src/c/*.c)
S_SRCS 		= $(wildcard ./src/asm/*.S)
C_OBJS 		= $(addprefix ./build/, $(notdir $(C_SRCS:%.c=%_c.o)))
S_OBJS 		= $(addprefix ./build/, $(notdir $(S_SRCS:%.S=%_s.o)))

INCLUDE_DIR	= include

CFLAGS 		= -Wall -O2 -ffreestanding -nostdlib
CC 			= aarch64-linux-gnu-gcc
LINKER 		= aarch64-linux-gnu-ld
OBJ_COPY 	= aarch64-linux-gnu-objcopy
EMULATOR	= qemu-system-aarch64
>>>>>>> de7a622e18bcb8ba0576355526bc99984106c5aa

.PHONY: clean run deploy

all: kernel8.img

<<<<<<< HEAD
start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

kernel8.img: start.o $(OBJS)
	$(LINKER) -nostdlib start.o $(OBJS) -T link.ld -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf kernel8.img
=======
build/%_s.o: src/asm/%.S
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

build/%_c.o: src/c/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@	

kernel8.img: $(S_OBJS) $(C_OBJS)
	$(LINKER) -nostdlib $(S_OBJS) $(C_OBJS) -T link.ld -o kernel8.elf
	$(OBJ_COPY) -O binary kernel8.elf kernel8.img
>>>>>>> de7a622e18bcb8ba0576355526bc99984106c5aa

clean:
	rm kernel8.elf kernel8.img start.o build/*.o >/dev/null 2>/dev/null || true

run: all
<<<<<<< HEAD
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio
=======
	$(EMULATOR) -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio
>>>>>>> de7a622e18bcb8ba0576355526bc99984106c5aa

deploy: all
	cp ./kernel8.img /run/media/brothre23/4DFF-0A36/
	sudo eject /dev/sdc