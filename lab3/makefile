.PHONY:all run clean onboard
APP:=kernel8
LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles
LIBS:=$(wildcard inc/*.c)
OBJS:=start.o main.o $(LIBS:.c=.o)

all:
	cd inc&&make
	make $(APP).img

$(APP).img:$(OBJS)
	aarch64-linux-gnu-ld -T $(LINKER) -o $(APP).elf $(OBJS)
	aarch64-linux-gnu-objcopy -O binary $(APP).elf $@
	#aarch64-linux-gnu-readelf -s $(APP).elf | grep __bss_end
	rm $(APP).elf

start.o:start.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<

%.o:%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<

run:
	make all
	#qemu-system-aarch64 -M raspi3 -kernel $(APP).img -display none -serial null -serial stdio -initrd ./archive/initramfs.cpio
	qemu-system-aarch64 -M raspi3 -kernel $(APP).img -display none -serial null -serial pty -initrd ./archive/initramfs.cpio

onboard:
	sudo screen /dev/ttyUSB0 115200

clean:
	cd inc&&make clean
	-rm $(APP).img
	-rm *.o
