all: clean start.img

start.img: start.elf
	aarch64-linux-gnu-objcopy -O binary start.elf start.img

start.elf: start.o
	aarch64-linux-gnu-ld -o start.elf start.o

start.o: start.S
	aarch64-linux-gnu-gcc-8 -c start.S

clean:
	rm start.img start.elf *.o >/dev/null 2>/dev/null || true