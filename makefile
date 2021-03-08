ARM = aarch64-linux-gnu
CC = $(ARM)-gcc
LD = $(ARM)-ld
OBJCOPY = $(ARM)-objcopy

IDIR = include
SDIR = src
BDIR = build

CFLAGS = -Wall -I $(IDIR) -g -ffreestanding

S_SRCS = $(wildcard $(SDIR)/*.S)
C_SRCS = $(wildcard $(SDIR)/*.c)
S_OBJS = $(S_SRCS:$(SDIR)/%.S=$(BDIR)/%.asmo)
C_OBJS = $(C_SRCS:$(SDIR)/%.c=$(BDIR)/%.o)

BL_LD = bl_linker.ld
KN_LD = kn_linker.ld

BL = bootloader
KN = kernel8

.PHONY: clean all

all: $(BL).img $(KN).elf

%.img: %.elf
	$(OBJCOPY) -O binary $< $@

$(BL).elf: $(S_OBJS) $(BL_LD) $(C_OBJS) 
	$(LD) -T $(BL_LD) --gc-sections -g -o $(BL).elf $(S_OBJS) $(C_OBJS)

$(KN).elf: $(S_OBJS) $(BOOT_LD) $(C_OBJS) 
	$(LD) -T $(KN_LD) --gc-sections -e kernel -g -o $(KN).elf $(S_OBJS) $(C_OBJS)

$(BDIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.asmo: $(SDIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BDIR)/*.asmo $(BDIR)/*.o *.elf $(BL).img initramfs.cpio

run: all
	qemu-system-aarch64 -M raspi3 -kernel $(BL).img -serial null -serial "pty" -display none

debug: all
	qemu-system-aarch64 -M raspi3 -kernel $(BL).img -serial null -serial "pty" -display none -S -s

burn: all
	lsblk | grep sdb1 > /dev/null && \
	sudo mount /dev/sdb1 sd_mount && \
	sudo cp $(BL).img sd_mount/kernel8.img && \
	sudo umount sd_mount 

genfs:
	cd rootfs && \
	find . | cpio -o -H newc > ../initramfs.cpio && \
	cd ..