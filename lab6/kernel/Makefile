TARGET_EXEC := kernel8

SRCS := $(shell for /f tokens^=* %%i in ('where *.c *.s')do echo %%~nxi)
OBJS := $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

all: $(TARGET_EXEC).img

%.o: %.c
	aarch64-none-linux-gnu-gcc -c -ffreestanding -mgeneral-regs-only $(flagcc) $<

%.o: %.s
	aarch64-none-linux-gnu-gcc -c -ffreestanding -mgeneral-regs-only $(flagcc) $<

$(TARGET_EXEC).img: $(OBJS) linker.ld
	aarch64-none-linux-gnu-ld -T linker.ld -o $(TARGET_EXEC).elf $(OBJS)
	aarch64-none-linux-gnu-objcopy -O binary $(TARGET_EXEC).elf $@

clean:
	del *.o $(TARGET_EXEC).elf $(TARGET_EXEC).img

run: $(TARGET_EXEC).img
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET_EXEC).img -display none -serial null -serial stdio -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb

debug: flagcc = -ggdb

debug: $(TARGET_EXEC).img
	qemu-system-aarch64 -M raspi3 -kernel $(TARGET_EXEC).img -display none -serial null -serial stdio -s -S -initrd initramfs.cpio -dtb bcm2710-rpi-3-b-plus.dtb
