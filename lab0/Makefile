all: kernel8.img

a.o: a.s
	aarch64-none-linux-gnu-gcc -c -ggdb a.s

kernel8.elf: a.o linker.ld
	aarch64-none-linux-gnu-ld -T linker.ld -o kernel8.elf a.o

kernel8.img: kernel8.elf
	aarch64-none-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	del *.o kernel8.elf kernel8.img

run: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
