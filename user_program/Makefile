all: clean start.elf

start.elf: start.o
	aarch64-linux-gnu-ld -o start.elf start.o

start.o: start.S
	aarch64-linux-gnu-gcc-8 -c start.S

clean:
	rm start.elf *.o >/dev/null 2>/dev/null || true