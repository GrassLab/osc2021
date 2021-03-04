# cross-compiler
COMPILER := aarch64-linux-gnu-gcc
LINKER := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy
QEMU := qemu-system-aarch64

DIR := src
BUILD_DIR := build
C_SRC := $(wildcard $(DIR)/*.c)
C_OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(C_SRC)))
ASM_SRC := $(wildcard $(DIR)/*.S)
ASM_OBJ := $(patsubst %.S, $(BUILD_DIR)/%.o, $(notdir $(ASM_SRC)))
CCFLAG := -Wall -nostdlib -Og -Isrc
ASMFLAG := -Isrc

ELF := kernel8.elf
LD := linker.ld
IMG := kernel8.img
PROVIED_IMG := nctuos.img
GDB := gdb-multiarch
MINI-UART := -serial null -serial stdio

all: $(IMG)

$(IMG): $(C_OBJ) $(ASM_OBJ)
	$(LINKER) -T $(LD) -o $(ELF) $(C_OBJ) $(ASM_OBJ)
	$(OBJCOPY) -O binary $(ELF) $(IMG)

$(BUILD_DIR)/%.o: $(DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -c $< -o $@

$(BUILD_DIR)/%.o: $(DIR)/%.S
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(ASMFLAG) -c $< -o $@

exe:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -serial null -serial stdio

dump:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -d in_asm

test:
	$(QEMU) -M raspi3 -kernel $(PROVIED_IMG) -display none -d in_asm

debug:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -S -s $(MINI-UART)

gdb:
	@echo "target remote :1234"
	$(GDB) $(IMG)

clean:
	rm -f $(ELF) $(IMG)
	rm -rf $(BUILD_DIR)
