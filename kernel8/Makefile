# cross-compiler
CC 			= aarch64-linux-gnu-gcc
LINKER 		= aarch64-linux-gnu-ld
OBJ_CPY 	= aarch64-linux-gnu-objcopy
QEMU		= qemu-system-aarch64



SRCDIR 		= src
INCLUDE_DIR = include
LIB_DIR 	= lib
LIB_HEADER	= lib/headers


SRCS = $(wildcard $(SRCDIR)/*.c )
OBJS = $(patsubst $(SRCDIR)/%.c, %.o, $(SRCS))
LINK_SCRIPT = linker.ld
KERNEL = kernel8
CFLAGS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -g 

# cpio archive
CPIO_DIR = rootfs
CPIO_FILES = $(wildcard $(CPIO_DIR)/*)
CPIO = initramfs.cpio
QEMU_CPIO = -initrd $(CPIO)

MINI-UART = -serial null -serial stdio
DTB = bcm2710-rpi-3-b-plus.dtb
QEMU_DTB = -dtb $(DTB)


VPATH = $(SRCDIR)
vpath %.c %.S $(SRCDIR) 

# cpio archive



all:  $(KERNEL).img



# include files
start.o: start.S
	$(CC) $(CFLAGS)  -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

kernel8.img: start.o  $(OBJS)
	$(LINKER) -nostdlib -nostartfiles start.o  $(OBJS) -T $(LINK_SCRIPT) -o kernel8.elf
	$(OBJ_CPY) -O binary kernel8.elf $@




# debug tools
cpio: $(CPIO_FILES)
	cd $(CPIO_DIR) && find . | cpio -o -H newc > ../$(CPIO)
	
debug:
	$(QEMU) -M raspi3 -kernel $(KERNEL).img   $(QEMU_CPIO) $(QEMU_DTB) $(MINI-UART)
	
clean:
	rm -f  *.o
	rm -f  $(KERNEL).elf $(KERNEL).img
	rm -rf $(BOOTLOADER).elf $(BOOTLOADER).img
	
