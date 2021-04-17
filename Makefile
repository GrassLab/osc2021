ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -g
ASMOPS = -Iinclude -g

RES_DIR = res
SRC_DIR = src
BOOT_SRC_DIR = src/bootloader
UTILS_SRC_DIR = utils
RAMFS = initramfs.cpio
PROGRAM_PATH = src/sub_program
DEBUG_PORT = 8000
##############
# PROGRAM_C_FILES = $(wildcard $(PROGRAM_PATH)/*.c)
# PROGRAM_NAMES = $(patsubst $(PROGRAM_PATH)/%.c,%,$(PROGRAM_C_FILES))
# PROGRAM_ASM_FILES = $(wildcard $(PROGRAM_PATH)/*.S)
# PROGRAM_FILES = $(PROGRAM_C_FILES:$(PROGRAM_PATH)/%.c=$(RES_DIR)/%_c.o)
# PROGRAM_FILES += $(PROGRAM_ASM_FILES:$(PROGRAM_PATH)/%.S=$(RES_DIR)/%_s.o)
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

all: kernel8.img bootloader.img $(RAMFS) sub_program.img

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

$(RES_DIR)/%_c.o: $(PROGRAM_PATH)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

$(RES_DIR)/%_s.o: $(PROGRAM_PATH)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -c $< -o $@
#############################################

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-ld -T $< -o kernel8.elf $(OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-objcopy kernel8.elf -O binary $@

# $(PROGRAM_NAMES).img: $(PROGRAM_PATH)/linker.ld $(PROGRAM_FILES) $(UTILS_OBJ_FILES)
# 	$(ARMGNU)-ld -T $< -o program/$(PROGRAM_NAMES).elf $(PROGRAM_FILES) $(UTILS_OBJ_FILES)
# 	$(ARMGNU)-objcopy program/$(PROGRAM_NAMES).elf -O binary program/$@

bootloader.img: $(BOOT_SRC_DIR)/bootloader.ld $(BOOT_OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-ld -T $< -o bootloader.elf $(BOOT_OBJ_FILES) $(UTILS_OBJ_FILES)
	$(ARMGNU)-objcopy bootloader.elf -O binary $@

$(RAMFS): rootfs
	cd rootfs; find . | cpio -o -H newc > ../$@; cd ..;

sub_program.img: $(RES_DIR)/test_s.o
	$(ARMGNU)-ld -o sub_program.elf $(RES_DIR)/test_s.o
	$(ARMGNU)-objcopy sub_program.elf -O binary $@

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img \
	-display none -serial null -serial stdio \
	-initrd $(RAMFS) \
	-dtb bcm2710-rpi-3-b-plus.dtb

run_subprogram:
	qemu-system-aarch64 -M raspi3 -kernel sub_program.img \
	-serial null -serial stdio \
	-display none -S -gdb tcp::$(DEBUG_PORT)

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img \
	-serial null -serial stdio \
	-display none -S -gdb tcp::$(DEBUG_PORT) \
	-initrd $(RAMFS) \
	-dtb bcm2710-rpi-3-b-plus.dtb

run_usb:
	python3 script/send_file.py -s kernel8.img
	screen /dev/ttyUSB0 115200

clean:
	rm -rf $(RES_DIR)/* *.img *.cpio *.elf program/*.elf program/*.img