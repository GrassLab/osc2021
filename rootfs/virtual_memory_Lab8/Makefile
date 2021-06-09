# Modify from 
# https://github.com/s-matyukevich/raspberry-pi-os/blob/master/src/lesson01/Makefile
ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude #-ggdb #-D__FS_DEBUG #-D__DEBUG 
ASMOPS = -Iinclude 

BUILD_DIR = build
SRC_DIR = src

all : kernel8.img

clean :
	rm -rf $(BUILD_DIR) *.img 

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary Lab8.img

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -serial null -drive if=sd,file=./sdcard/sfn_nctuos.img,format=raw -display none -S -s 

run_cpio:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio

# run with sd card (Lab7)
run_sd:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -drive if=sd,file=./sdcard/sfn_nctuos.img,format=raw

# run with cpio and sd card
run_all:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio -drive if=sd,file=./sdcard/sfn_nctuos.img,format=raw