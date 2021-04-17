ARM = aarch64-linux-gnu
CC = $(ARM)-gcc
LD = $(ARM)-ld
OBJCOPY = $(ARM)-objcopy
MAKE = make

IDIR = include
SDIR = src
BDIR = build

ROOTFS = rootfs
MOUNT_DIR = sd_mount
SDB = /dev/sdb1
INITRAMFS = initramfs.cpio
DTB = bcm2710-rpi-3-b-plus.dtb

CFLAGS = -Wall -I $(IDIR) -g -ffreestanding -march=armv8-a -O2

S_SRCS = $(wildcard $(SDIR)/*.S)
C_SRCS = $(wildcard $(SDIR)/*.c)
S_OBJS = $(S_SRCS:$(SDIR)/%.S=$(BDIR)/%.asmo)
C_OBJS = $(C_SRCS:$(SDIR)/%.c=$(BDIR)/%.o)

BL_LD = bl_linker.ld
KN_LD = kn_linker.ld

BL = bootloader
KN = kernel8

UDIR = usr

.PHONY: clean USR

all: $(BDIR) $(BL).img $(KN).elf USR $(INITRAMFS)

%.img: %.elf
	$(OBJCOPY) -O binary $< $@

$(BL).elf: $(S_OBJS) $(BL_LD) $(C_OBJS) 
	$(LD) -T $(BL_LD) --gc-sections -g -o $(BL).elf $(S_OBJS) $(C_OBJS)

$(KN).elf: $(S_OBJS) $(KN_LD) $(C_OBJS) 
	$(LD) -T $(KN_LD) --gc-sections -e kernel -g -o $(KN).elf $(S_OBJS) $(C_OBJS)

$(BDIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.asmo: $(SDIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BDIR)/*.asmo $(BDIR)/*.o *.elf $(BL).img $(INITRAMFS) $(UDIR)/*.o $(UDIR)/*.elf $(UDIR)/*.img

run: all
	qemu-system-aarch64 -M raspi3 -kernel $(BL).img -initrd $(INITRAMFS) \
	-dtb $(DTB) -serial null -serial "pty" -display none

debug: all
	qemu-system-aarch64 -M raspi3 -kernel $(BL).img -initrd $(INITRAMFS) \
	-dtb $(DTB) -serial null -serial "pty" -display none -S -s

burn: $(SDB) $(MOUNT_DIR) all
	sudo mount $(SDB) $(MOUNT_DIR) && \
	sudo cp $(BL).img $(MOUNT_DIR)/$(BL).img && \
	sudo cp $(INITRAMFS) $(MOUNT_DIR)/$(INITRAMFS) && \
	sudo cp config.txt $(MOUNT_DIR)/config.txt && \
	sudo cp $(DTB) $(MOUNT_DIR)/$(DTB) && \
	sudo umount $(MOUNT_DIR) 

$(MOUNT_DIR):
	mkdir $(MOUNT_DIR)

$(ROOTFS):
	mkdir $(ROOTFS)

$(INITRAMFS): $(UDIR)/*
	rm -rf $(ROOTFS)
	cp -r $(UDIR) $(ROOTFS)
	
	cd $(ROOTFS) && \
	find . | cpio -o -H newc > ../$(INITRAMFS) && \
	cd ..

genfs:
	cd $(ROOTFS) && \
	find . | cpio -o -H newc > ../$(INITRAMFS) && \
	cd ..

$(BDIR):
	mkdir $(BDIR)

USR: $(UDIR)
	$(MAKE) -C $(UDIR)