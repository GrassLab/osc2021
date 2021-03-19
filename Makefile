ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude

RES_DIR = res
SRC_DIR = src
BOOT_SRC_DIR = src/bootloader
UTILS_SRC_DIR = utils
RAMFS = initramfs.cpio
##############
C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(RES_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(RES_DIR)/%_s.o)
##############
BOOT_C_FILES = $(wildcard $(BOOT_SRC_DIR)/*.c)
BOOT_ASM_FILES = $(wildcard $(BOOT_SRC_DIR)/*.S)
BOOT_OBJ_FILES = $(BOOT_C_FILES:$(BOOT_SRC_DIR)/%.c=$(RES_DIR)/%_c.o)
BOOT_OBJ_FILES += $(BOOT_ASM_FILES:$(BOOT_SRC_DIR)/%.S=$(RES_DIR)/%_s.o)
##############
UTILS_C_FILES = $(wildcard $(UTILS_SRC_DIR)/*.c)
UTILS_ASM_FILES = $(wildcard $(UTILS_SRC_DIR)/*.S)
UTILS_OBJ_FILES = $(UTILS_C_FILES:$(UTILS_SRC_DIR)/%.c=$(RES_DIR)/%_c.o)
UTILS_OBJ_FILES += $(UTILS_ASM_FILES:$(UTILS_SRC_DIR)/%.S=$(RES_DIR)/%_s.o)
##############

.PHONY: clean run

all: kernel8.img bootloader.img $(RAMFS)

#############################################
$(RES_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(RES_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(RES_DIR)/%_c.o: $(BOOT_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(RES_DIR)/%_s.o: $(BOOT_SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

$(RES_DIR)/%_c.o: $(UTILS_SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(RES_DIR)/%_s.o: $(UTILS_SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@
#############################################

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-ld -T $< -o $(RES_DIR)/kernel8.elf $(OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-objcopy $(RES_DIR)/kernel8.elf -O binary $@

bootloader.img: $(BOOT_SRC_DIR)/bootloader.ld $(BOOT_OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-ld -T $< -o $(RES_DIR)/bootloader.elf $(BOOT_OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-objcopy $(RES_DIR)/bootloader.elf -O binary $@

$(RAMFS): rootfs
	cd rootfs; find . | cpio -o -H newc > ../$@; cd ..;

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd $(RAMFS)

clean:
	rm -rf $(RES_DIR)/* *.img *.cpio