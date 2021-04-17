ARMGNU = aarch64-linux-gnu
QEMU = qemu-system-aarch64
# -Iinclude tells it to look for header files in the include folder.
# -fPIC makes the addresses relative instead of absolute allowing us to place the kernel anywhere in memory.
COPS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -Iinclude -g
ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src

KERNEL = kernel8
Linker = linker.ld

CPIO_DIR = rootfs
CPIO_FILES = $(wildcard $(CPIO_DIR)/*)
CPIO = initramfs.cpio
QEMU_CPIO = -initrd archive/$(CPIO)

MINI-UART = -serial null -serial stdio
DTB = bcm2710-rpi-3-b-plus.dtb
QEMU_DTB = -dtb $(DTB)


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


$(KERNEL).img: $(SRC_DIR)/$(Linker) $(OBJ_FILES)
	$(ARMGNU)-ld -T $(SRC_DIR)/$(Linker) -o $(BUILD_DIR)/$(KERNEL).elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/$(KERNEL).elf -O binary $(KERNEL).img
	


run:
	#$(QEMU) -M raspi3 -kernel $(KERNEL).img -serial null -serial stdio
	$(QEMU) -M raspi3 -kernel $(KERNEL).img   $(QEMU_CPIO) $(QEMU_DTB) $(MINI-UART)


# debug tools
cpio: $(CPIO_FILES)
	cd $(CPIO_DIR) && find . | cpio -o -H newc > ../$(CPIO)
	


gdb:
	qemu-system-aarch64 -M raspi3 -kernel $(KERNEL).img -S -s $(QEMU_CPIO) $(QEMU_DTB)