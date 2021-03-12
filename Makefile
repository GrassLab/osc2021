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

# bootloader
BOOT_DIR := bootloader/src
BOOT_BUILD := build/bootloader
BOOT_C_SRC := $(wildcard $(BOOT_DIR)/*.c)
BOOT_C_OBJ := $(patsubst %.c, $(BOOT_BUILD)/%.o, $(notdir $(BOOT_C_SRC)))
BOOT_ASM_SRC := $(wildcard $(BOOT_DIR)/*.S)
BOOT_ASM_OBJ := $(patsubst %.S, $(BOOT_BUILD)/%.o, $(notdir $(BOOT_ASM_SRC)))
BOOT_LD := bootloader/linker.ld
BOOTLOADER = bootloader

# kernel
DIR := src
BUILD_DIR := build
C_SRC := $(wildcard $(DIR)/*.c)
C_OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(C_SRC)))
ASM_SRC := $(wildcard $(DIR)/*.S)
ASM_OBJ := $(patsubst %.S, $(BUILD_DIR)/%.o, $(notdir $(ASM_SRC)))
LD := linker.ld
KERNEL := kernel8

GDB := gdb-multiarch
MINI-UART := -serial null -serial stdio

# flag
TEST_IMG := $(BOOTLOADER).img
UART := UART_MINI # UART_MINI or UART_PL011
CCFLAG := -Wall -nostdlib -Og -D$(UART) -I$(INC_DIR)
ASMFLAG := -Isrc

all: $(BOOTLOADER).img $(KERNEL).img

# include files
$(INC_BUILD)/%.o: $(INC_DIR)/%.c
	mkdir -p $(INC_BUILD)
	$(COMPILER) $(CCFLAG) -c $< -o $@

$(INC_BUILD)/%.o: $(INC_DIR)/%.S
	mkdir -p $(INC_BUILD)
	$(COMPILER) $(CCFLAG) -c $< -o $@

# bootloader
$(BOOTLOADER).img: $(INC_C_OBJ) $(INC_ASM_OBJ) $(BOOT_C_OBJ) $(BOOT_ASM_OBJ)
	$(LINKER) -T $(BOOT_LD) -o $(BOOTLOADER).elf $(INC_C_OBJ) $(INC_ASM_OBJ) $(BOOT_C_OBJ) $(BOOT_ASM_OBJ)
	$(OBJCOPY) -O binary $(BOOTLOADER).elf $(BOOTLOADER).img

$(BOOT_BUILD)/%.o: $(BOOT_DIR)/%.c
	@mkdir -p $(BOOT_BUILD)
	$(COMPILER) $(CCFLAG) -I$(BOOT_DIR) -c $< -o $@

$(BOOT_BUILD)/%.o: $(BOOT_DIR)/%.S
	@mkdir -p $(BOOT_BUILD)
	$(COMPILER) $(CCFLAG) -I$(BOOT_DIR) -c $< -o $@

# kernel
$(KERNEL).img: $(INC_C_OBJ) $(INC_ASM_OBJ) $(C_OBJ) $(ASM_OBJ)
	$(LINKER) -T $(LD) -o $(KERNEL).elf $(INC_C_OBJ) $(INC_ASM_OBJ) $(C_OBJ) $(ASM_OBJ)
	$(OBJCOPY) -O binary $(KERNEL).elf $(KERNEL).img

$(BUILD_DIR)/%.o: $(DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -I$(DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DIR)/%.S
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -I$(DIR) -c $< -o $@

# debug tools
exe:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) -display none -serial null -serial pty

dump:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) -display none -d in_asm

debug:
	$(QEMU) -M raspi3 -kernel $(TEST_IMG) -display none -S -s $(MINI-UART)

gdb:
	@echo "target remote :1234"
	$(GDB) $(IMG)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(KERNEL).elf $(KERNEL).img
	rm -rf $(BOOTLOADER).elf $(BOOTLOADER).img
