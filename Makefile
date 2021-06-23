# cross-compiler
COMPILER := aarch64-linux-gnu-gcc
LINKER := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy
QEMU := qemu-system-aarch64

# kernel
DIR := src
BUILD_DIR := build
C_SRC := $(wildcard $(DIR)/*.c)
C_OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(C_SRC)))
ASM_SRC := $(wildcard $(DIR)/*.S)
ASM_OBJ := $(patsubst %.S, $(BUILD_DIR)/%.o, $(notdir $(ASM_SRC)))
LD := linker.ld
TARGET := kernel8

GDB := gdb-multiarch
MINI-UART := -serial null -serial stdio

# APP
APP_DIR := app

# cpio archive
CPIO_DIR := rootfs
CPIO_FILES := $(wildcard $(CPIO_DIR)/*)
CPIO := initramfs.cpio
QEMU_CPIO := -initrd $(CPIO)

# flattened devicetree (dtb)
DTB := config/bcm2710-rpi-3-b-plus.dtb
QEMU_DTB := -dtb $(DTB)

# libary
LIB_DIR := lib
LIB_C_SRC := $(wildcard $(LIB_DIR)/src/*.c)
LIB_C_OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(LIB_C_SRC)))
LIB_ASM_SRC := $(wildcard $(LIB_DIR)/src/*.S)
LIB_ASM_OBJ := $(patsubst %.S, $(BUILD_DIR)/%.o, $(notdir $(LIB_ASM_SRC)))

# flag
BOOTLOADER := bootloader
#TEST_IMG := $(BOOTLOADER).img
TEST_IMG := $(TARGET).img
UART := UART_MINI # UART_MINI or UART_PL011
CCFLAG := -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles -Og -g -D$(UART) -I$(DIR) -I$(LIB_DIR)/include

all:  APP $(BOOTLOADER).img $(TARGET).img $(CPIO)

# kernel
$(TARGET).img: $(C_OBJ) $(ASM_OBJ) $(LIB_C_OBJ) $(LIB_ASM_OBJ)
	$(LINKER) -T $(LD) -o $(TARGET).elf $(C_OBJ) $(ASM_OBJ) $(LIB_C_OBJ) $(LIB_ASM_OBJ)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).img

$(BUILD_DIR)/%.o: $(DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -c $< -o $@

$(BUILD_DIR)/%.o: $(DIR)/%.S
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -c $< -o $@

# libary
$(BUILD_DIR)/%.o: $(LIB_DIR)/src/%.c
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/src/%.S
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -c $< -o $@

# apps
APP:
	cd lib && make
	cd $(APP_DIR) && make


# bootloader
$(BOOTLOADER).img:
	@cd $(BOOTLOADER) && make
	@cp $(BOOTLOADER)/$(BOOTLOADER).img ./
	@cp $(BOOTLOADER)/$(BOOTLOADER).elf ./

# cpio archive
$(CPIO): $(CPIO_FILES)
	cd $(CPIO_DIR) && find . | cpio -o -H newc > ../$(CPIO)

# debug tools
exe:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) $(QEMU_CPIO) $(QEMU_DTB) -display none -serial null -serial pty -d int -drive if=sd,file=sfn_nctuos.img,format=raw

dump:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) -display none -d in_asm

debug:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) $(QEMU_CPIO) $(QEMU_DTB) -display none -serial null -serial pty -S -s $(MINI-UART) -d int -drive if=sd,file=sfn_nctuos.img,format=raw

gdb:
	@echo "target remote :1234"
	$(GDB) $(IMG)

clear:
	rm -rf $(BUILD_DIR)
	rm -rf $(CPIO)
	rm -f $(TARGET).elf $(TARGET).img
	rm -f $(BOOTLOADER).elf $(BOOTLOADER).img
	@cd $(BOOTLOADER) && make clean

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(CPIO)
	rm -f $(TARGET).elf $(TARGET).img
	cd $(APP_DIR) && make clean
	cd lib && make clean
