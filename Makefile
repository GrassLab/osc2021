ARMGNU = aarch64-linux-gnu
ifeq ($(DEBUG), 1)
FLAGS = -Wall -nostdlib -ffreestanding -Werror -ggdb3 -O0
else
FLAGS = -Wall -nostdlib -ffreestanding -Werror
endif

INCLUDES = -Iinclude/bootloader -Iinclude/kernel -Iinclude/lib

BUILD_DIR= build
SRC_DIR = src
OBJS_DIR = objs

C_FILES = $(wildcard $(SRC_DIR)/*/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*/*.S)

OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(OBJS_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(OBJS_DIR)/%_s.o)

all: kernel8.img bootloader.img app assets

$(OBJS_DIR)/%_c.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
ifeq ($(DEBUG), 1)
	$(ARMGNU)-gcc $(FLAGS) $(INCLUDES) -D EMU -c $< -o $@
else
	$(ARMGNU)-gcc $(FLAGS) $(INCLUDES) -c $< -o $@
endif


$(OBJS_DIR)/%_s.o: $(SRC_DIR)/%.S
	@mkdir -p $(@D)
ifeq ($(DEBUG), 1)
	$(ARMGNU)-gcc $(FLAGS) $(INCLUDES) -D EMU -c $< -o $@
else 
	$(ARMGNU)-gcc $(FLAGS) $(INCLUDES) -c $< -o $@
endif

kernel8.img: $(filter $(OBJS_DIR)/kernel/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES)) $(SRC_DIR)/kernel/linker.ld
	@mkdir -p $(BUILD_DIR)
	@$(ARMGNU)-ld -T $(SRC_DIR)/kernel/linker.ld -o $(BUILD_DIR)/kernel8.elf $(filter $(OBJS_DIR)/kernel/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES))
	@$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(BUILD_DIR)/$@

bootloader.img: $(filter $(OBJS_DIR)/bootloader/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES)) $(SRC_DIR)/bootloader/linker.ld
	@mkdir -p $(BUILD_DIR)	
	@$(ARMGNU)-ld -T $(SRC_DIR)/bootloader/linker.ld -o $(BUILD_DIR)/bootloader.elf $(filter $(OBJS_DIR)/bootloader/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES))
	@$(ARMGNU)-objcopy $(BUILD_DIR)/bootloader.elf -O binary $(BUILD_DIR)/$@

app: $(filter $(OBJS_DIR)/app/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES)) $(SRC_DIR)/app/linker.ld
	@mkdir -p $(BUILD_DIR)	
	@$(ARMGNU)-ld -T $(SRC_DIR)/app/linker.ld -o $(BUILD_DIR)/app.elf $(filter $(OBJS_DIR)/app/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES))
	@$(ARMGNU)-objcopy $(BUILD_DIR)/app.elf -O binary rootfs/$@

assets: $(SRC_DIR)/config.txt $(wildcard rootfs/*)
	@mkdir -p $(BUILD_DIR)
	@cp $< $(BUILD_DIR)
	@cd rootfs; find . | cpio -o -H newc > ../${BUILD_DIR}/initramfs.cpio



clean:
	rm -rf $(BUILD_DIR) $(OBJS_DIR)

#TODO: set emu tag when compiling debuging code
run:
	qemu-system-aarch64 \
		-M raspi3 \
		-kernel $(BUILD_DIR)/bootloader.img \
		-initrd $(BUILD_DIR)/initramfs.cpio \
		-drive if=sd,file=sfn_nctuos.img,format=raw \
		-serial null \
		-serial pty \
		-display none

run-debug:
	qemu-system-aarch64 -s -S -M raspi3 -kernel $(BUILD_DIR)/kernel8.img -initrd $(BUILD_DIR)/initramfs.cpio -serial null -serial stdio -display none

.PHONY: mount
mount:
	sudo mount -t msdos -o loop,rw,sync,offset=1048576 ./sfn_nctuos.img mount/
umount:
	sudo umount mount