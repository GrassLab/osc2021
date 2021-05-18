ARMGNU = aarch64-linux-gnu
QEMU = qemu-system-aarch64
# -Iinclude tells it to look for header files in the include folder.
# -fPIC makes the addresses relative instead of absolute allowing us to place the kernel anywhere in memory.
COPS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude
ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src

KERNEL = bootloader
Linker = linker.ld

all : clean $(KERNEL).img

clean :
	rm -rf $(BUILD_DIR) *.img
	
$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -c $< -o $@
	
$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	$(ARMGNU)-gcc $(COPS) -c $< -o $@


ASM_FILES = $(wildcard $(SRC_DIR)/*.S) 
C_FILES = $(wildcard $(SRC_DIR)/*.c) 
OBJ_FILES = $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)
OBJ_FILES += $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

$(KERNEL).img: $(SRC_DIR)/$(Linker) $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/$(Linker) -o $(BUILD_DIR)/$(KERNEL).elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/$(KERNEL).elf -O binary $(KERNEL).img

run:
	$(QEMU) -M raspi3 -kernel $(KERNEL).img -serial null -serial stdio
