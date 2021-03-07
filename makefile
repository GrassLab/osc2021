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

BOOT_LD = bl_linker.ld
KERN_LD = kn_linker.ld

.PHONY: all

all: clean bootloader.img kernel8.elf

bootloader.img: bootloader.elf
	$(OBJCOPY) -O binary bootloader.elf bootloader.img

bootloader.elf: $(S_OBJS) $(BOOT_LD) $(C_OBJS) 
	$(LD) -T $(BOOT_LD) --gc-sections -g -o bootloader.elf $(S_OBJS) $(C_OBJS)

kernel8.elf: $(S_OBJS) $(BOOT_LD) $(C_OBJS) 
	$(LD) -T $(KERN_LD) --gc-sections -e kernel -g -o kernel8.elf $(S_OBJS) $(C_OBJS)

$(BDIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.asmo: $(SDIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BDIR)/*.asmo $(BDIR)/*.o *.elf kernel8.img bootloader.img

run: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial "pty" -display none

debug: all
	qemu-system-aarch64 -M raspi3 -kernel bootloader.img -serial null -serial "pty" -display none -S -s

burn: all
	lsblk | grep sdb1 > /dev/null && \
	sudo mount /dev/sdb1 sd_mount && \
	sudo cp bootloader.img sd_mount/kernel8.img && \
	sudo umount sd_mount 
