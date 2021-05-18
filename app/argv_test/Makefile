TARGET := argv_test

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
LD := ../linker.ld

# libary
LIB_DIR := ../../lib
LIB_OBJ := $(wildcard $(LIB_DIR)/build/*.o)

# flag
CCFLAG := -Wall -ffreestanding -nostdinc -nostdlib -nostartfiles -Og -g -I$(DIR) -I$(LIB_DIR)/include

all: $(TARGET)
	cp $(TARGET) ../../rootfs

$(TARGET): $(C_OBJ) $(ASM_OBJ)
	$(LINKER) -T $(LD) -o $(TARGET).o $(C_OBJ) $(ASM_OBJ) $(LIB_OBJ)
	$(OBJCOPY) -O binary $(TARGET).o $(TARGET)

$(BUILD_DIR)/%.o: $(DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -c $< -o $@

$(BUILD_DIR)/%.o: $(DIR)/%.S
	@mkdir -p $(BUILD_DIR)
	$(COMPILER) $(CCFLAG) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(TARGET).o
