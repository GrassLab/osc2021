all: start.o main.o uart.o allocater.o exception_c.o exception_s.o kernel8.img 

start.o: start.S
	aarch64-linux-gnu-gcc -c start.S -o start.o

main.o: main.c
	aarch64-linux-gnu-gcc -c main.c -o main.o

uart.o: uart.c
	aarch64-linux-gnu-gcc -c uart.c -o uart.o

allocater.o: allocater.c
	aarch64-linux-gnu-gcc -c allocater.c -o allocater.o

exception_s.o: exception.S
	aarch64-linux-gnu-gcc -c exception.S -o exception_s.o

exception_c.o: exception.c
	aarch64-linux-gnu-gcc -c exception.c -o exception_c.o

kernel8.img: start.o main.o uart.o allocater.o exception_s.o exception_c.o
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf start.o main.o uart.o allocater.o exception_c.o exception_s.o
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm -rf kernel8.elf
	rm -rf *.o

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -initrd initramfs.cpio 

runpty:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty -initrd initramfs.cpio 

runasm:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm -serial null -serial stdio -initrd initramfs.cpio

rundis:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio -initrd initramfs.cpio

debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -S -s -initrd initramfs.cpio 

gdb:
	gdb-multiarch -x ./debug.gdb