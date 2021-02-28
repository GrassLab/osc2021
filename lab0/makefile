.PHONY:all run clean
APP:=a
LINKER:=linker.ld
CFLAGS:=-Wall -ffreestanding -nostdinc -nostdlib -nostartfiles
LFLAGS:=-nostdlib -nostartfiles


all:
	make $(APP).img

$(APP).img:$(APP).elf
	aarch64-linux-gnu-objcopy -O binary $< $@

$(APP).elf:$(APP).o
	aarch64-linux-gnu-ld $(LFLAGS) -T $(LINKER) -o $@ $<

$(APP).o:$(APP).S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<

run:
	make $(APP).img
	qemu-system-aarch64 -M raspi3 -kernel $(APP).img -display none -d in_asm

clean:
	-rm $(APP).img
	-rm $(APP).elf
	-rm $(APP).o
