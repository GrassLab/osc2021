ifeq ($(OS), Windows_NT)
	GNU_OBJECT=aarch64-none-linux-gnu-objcopy
	GNU_LD=aarch64-none-linux-gnu-ld
	GNU_GCC=aarch64-none-linux-gnu-gcc
	DEL=del
else
	GNU_OBJECT=aarch64-linux-gnu-objcopy
	GNU_LD=aarch64-linux-gnu-ld
	GNU_GCC=aarch64-linux-gnu-gcc
	DEL=rm -f
endif



kernel8.img: kernel8.elf
	$(GNU_OBJECT) -O binary kernel8.elf kernel8.img

kernel8.elf: a.o linker.ld 
	$(GNU_LD) -T linker.ld -o kernel8.elf a.o

a.o: a.S
	$(GNU_GCC) -c a.S

clean:
	$(DEL) a.o kernel8.elf kernel8.img

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s 
