# cross-compiler
COMPILER := aarch64-linux-gnu-gcc
LINKER := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy
QEMU := qemu-system-aarch64

# include
INC_DIR := include
INC_BUILD := build/include
INC_C_SRC := $(wildcard $(INC_DIR)/*.c)
INC_C_OBJ := $(patsubst %.c, $(INC_BUILD)/%.o, $(notdir $(INC_C_SRC)))
INC_ASM_SRC := $(wildcard $(INC_DIR)/*.S)
INC_ASM_OBJ := $(patsubst %.S, $(INC_BUILD)/%.o, $(notdir $(INC_ASM_SRC)))

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

# flag
BOOTLOADER := bootloader
TEST_IMG := $(BOOTLOADER).img
UART := UART_MINI # UART_MINI or UART_PL011
CCFLAG := -Wall -nostdlib -Og -D$(UART) -I$(INC_DIR)
ASMFLAG := -Isrc

# cpio archive
CPIO_DIR := rootfs
CPIO_FILES := $(wildcard $(CPIO_DIR)/*)
CPIO := initramfs.cpio
QEMU_CPIO := -initrd $(CPIO)

# flattened devicetree (dtb)
DTB := bcm2710-rpi-3-b-plus.dtb
QEMU_DTB := -dtb $(DTB)

all:  $(BOOTLOADER).img $(TARGET).img $(CPIO)

# include files
$(INC_BUILD)/%.o: $(INC_DIR)/%.c
	mkdir -p $(INC_BUILD)
	$(COMPILER) $(CCFLAG) -c $< -o $@

$(INC_BUILD)/%.o: $(INC_DIR)/%.S
	mkdir -p $(INC_BUILD)
	$(COMPILER) $(CCFLAG) -c $< -o $@

# kernel
$(TARGET).img: $(INC_C_OBJ) $(INC_ASM_OBJ) $(C_OBJ) $(ASM_OBJ)
	$(LINKER) -T $(LD) -o $(TARGET).elf $(INC_C_OBJ) $(INC_ASM_OBJ) $(C_OBJ) $(ASM_OBJ)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).img

$(BUILD_DIR)/%.o: $(DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -I$(DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DIR)/%.S
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -I$(DIR) -c $< -o $@

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
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) $(QEMU_CPIO) $(QEMU_DTB) -display none -serial null -serial pty

dump:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) -display none -d in_asm

debug:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) $(QEMU_CPIO) $(QEMU_DTB) -display none -S -s $(MINI-UART)

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
