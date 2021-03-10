ARMGNU = aarch64-linux-gnu
FLAGS = -Wall -nostdlib -Iinclude -ffreestanding -Werror

BUILD_DIR= build
SRC_DIR = src
OBJS_DIR = objs

C_FILES = $(wildcard $(SRC_DIR)/*/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*/*.S)

OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(OBJS_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(OBJS_DIR)/%_s.o)

all: kernel8.img bootloader.img

$(OBJS_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(FLAGS) -c $< -o $@

$(OBJS_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(FLAGS) -c $< -o $@

kernel8.img: $(filter $(OBJS_DIR)/kernel/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES))
	mkdir -p $(BUILD_DIR)
	$(ARMGNU)-ld -T $(SRC_DIR)/kernel/linker.ld -o $(BUILD_DIR)/kernel8.elf $(filter $(OBJS_DIR)/kernel/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES))
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary $(BUILD_DIR)/$@

bootloader.img: $(filter $(OBJS_DIR)/bootloader/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES))
	mkdir -p $(BUILD_DIR)	
	$(ARMGNU)-ld -T $(SRC_DIR)/bootloader/linker.ld -o $(BUILD_DIR)/bootloader.elf $(filter $(OBJS_DIR)/bootloader/%.o $(OBJS_DIR)/lib/%.o, $(OBJ_FILES))
	$(ARMGNU)-objcopy $(BUILD_DIR)/bootloader.elf -O binary $(BUILD_DIR)/$@




clean:
	rm -rf $(BUILD_DIR) $(OBJS_DIR)

run:
	qemu-system-aarch64 -M raspi3 -kernel $(BUILD_DIR)/kernel8.img -serial null -serial stdio -display none