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

all: clean kernel8.img

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

kernel8.elf: $(S_OBJS) linker.ld $(C_OBJS) 
	$(LD) -T linker.ld -g -o kernel8.elf $(S_OBJS) $(C_OBJS)

$(BDIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.asmo: $(SDIR)/%.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BDIR)/*.asmo $(BDIR)/*.o kernel8.elf kernel8.img

run: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio -display none

debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio -display none -S -s

tty: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial "pty" -display none

burn: all
	lsblk | grep sdb1 > /dev/null && sudo mount /dev/sdb1 sd_mount && sudo cp kernel8.img sd_mount/kernel8.img && sudo umount sd_mount 
