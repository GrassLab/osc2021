CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
# %.o: %.c
# 	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

kernel8.img: boot.o utils.o uart.o mm.o cutils.o test.o main.o
	aarch64-linux-gnu-ld -nostdlib -nostartfiles -Iinclude boot.o utils.o uart.o mm.o cutils.o test.o main.o -T linker.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

boot.o: boot.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c boot.S -o boot.o

utils.o: utils.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c utils.S -o utils.o

uart.o: uart.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c uart.c -o uart.o

mm.o: mm.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c mm.c -o mm.o

cutils.o: cutils.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c cutils.c -o cutils.o

test.o: test.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c test.c -o test.o

main.o: main.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c main.c -o main.o

clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true
