ARMGNU = aarch64-linux-gnu
COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -std=gnu99 -ggdb -MMD
BUILD_DIR = build


all: user_program
clean :
	rm -rf $(BUILD_DIR) *.o 

$(BUILD_DIR)/%_c.o: %.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -fno-zero-initialized-in-bss -c $< -o $@

$(BUILD_DIR)/%_s.o: %.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -fno-zero-initialized-in-bss -c $< -o $@

C_FILES = $(wildcard *.c)
ASM_FILES = $(wildcard *.S)
OBJ_FILES = $(C_FILES:%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

user_program: linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -T linker.ld -o $(BUILD_DIR)/user_program.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/user_program.elf -O binary user_program

