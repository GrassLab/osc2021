CC:= aarch64-linux-gnu-gcc
LD:= aarch64-linux-gnu-ld
OBJCPY:= aarch64-linux-gnu-objcopy
Source:= a.S
LINKER:= linker.ld
QEMUL:= qemu-system-aarch64

.PHONY: all
all: kernel8.img
	$(QEMUL) -M raspi3 -kernel kernel8.img -display none -d in_asm

kernel8.img: kernel8.elf
	$(OBJCPY) -O binary kernel8.elf kernel8.img

kernel8.elf: a.o
	$(LD) -T $(LINKER) -o kernel8.elf a.o 

%.o: %.S
	$(CC) -c $< -o $@ 


.PHONY: clean
clean:
	rm -f a.o kernel8.elf kernel8.img a.out


