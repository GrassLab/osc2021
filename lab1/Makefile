all: start.o main.o uart.o kernel8.img 

start.o: start.S
	aarch64-linux-gnu-gcc -c start.S -o start.o

main.o: main.c
	aarch64-linux-gnu-gcc -c main.c -o main.o

uart.o: uart.c
	aarch64-linux-gnu-gcc -c uart.c -o uart.o

kernel8.img: start.o main.o uart.o
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf start.o main.o uart.o
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm -rf kernel8.elf
	rm -rf *.o

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio