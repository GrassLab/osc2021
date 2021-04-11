CC = aarch64-linux-gnu-gcc
# ASMFLAGS = 
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
OBJCOPYFLAGS = -O binary


BUILD_DIR = build
SRC_DIR = src

all: user_program.img

clean:
	rm $(BUILD_DIR)/*

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(CC) $(ASMFLAGS) -c $< -o $@

ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

user_program.elf: $(OBJ_FILES)
	$(LD) -o $(BUILD_DIR)/user_program.elf $(OBJ_FILES)

user_program.img: user_program.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) $(BUILD_DIR)/user_program.elf $(BUILD_DIR)/user_program.img
	cp build/user_program.img ../kernel/rootfs && \
	cd ../kernel/rootfs && \
	rm -f ../initramfs.cpio && \
	find . | cpio -o -H newc > ../initramfs.cpio

dump:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/user_program.img -display none -d in_asm
