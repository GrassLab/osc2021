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
TARGET := bootloader

GDB := gdb-multiarch
MINI-UART := -serial null -serial stdio

# flag
TEST_IMG := bootloader.img
#TEST_IMG := $(TARGET).img
UART := UART_MINI # UART_MINI or UART_PL011
ifeq ($(tar), raspi3)
TAR = M_RASPI3
else
TAR = M_QEMU
endif
CCFLAG := -Wall -nostdlib -Og -D$(UART) -D$(TAR) -I$(INC_DIR)
ASMFLAG := -Isrc


all: $(TARGET).img $(CPIO)

# include files
$(INC_BUILD)/%.o: $(INC_DIR)/%.c
	mkdir -p $(INC_BUILD)
	$(COMPILER) $(CCFLAG) -c $< -o $@

$(INC_BUILD)/%.o: $(INC_DIR)/%.S
	mkdir -p $(INC_BUILD)
	$(COMPILER) $(CCFLAG) -c $< -o $@

# bootloader
$(TARGET).img: $(INC_C_OBJ) $(INC_ASM_OBJ) $(C_OBJ) $(ASM_OBJ)
	$(LINKER) -T $(LD) -o $(TARGET).elf $(INC_C_OBJ) $(INC_ASM_OBJ) $(C_OBJ) $(ASM_OBJ)
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).img

$(BUILD_DIR)/%.o: $(DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -I$(DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(DIR)/%.S
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -I$(DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET).elf $(TARGET).img
