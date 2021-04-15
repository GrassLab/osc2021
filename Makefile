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

.PHONY: clean run deploy

all: kernel8.img

build/%_s.o: src/asm/%.S
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

build/%_c.o: src/c/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@	

kernel8.img: $(S_OBJS) $(C_OBJS)
	$(LINKER) -nostdlib $(S_OBJS) $(C_OBJS) -T link.ld -o kernel8.elf
	$(OBJ_COPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf kernel8.img start.o build/*.o >/dev/null 2>/dev/null || true

run: all
	$(EMULATOR) -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio

deploy: all
	cp ./kernel8.img /run/media/brothre23/4DFF-0A36/
	sudo eject /dev/sdc