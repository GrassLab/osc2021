CFLAGS = -Wall -g -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles
# %.o: %.c
# 	aarch64-linux-gnu-gcc $(CFLAGS) -c $< -o $@

kernel8.img: boot.o utils.o uart.o mm.o cutils.o dtp.o test.o syscall.o entry.o irq.o cirq.o csched.o sched.o syslib.o vfs.o tmpfs.o procfs.o main.o
	aarch64-linux-gnu-ld -nostdlib -nostartfiles -Iinclude boot.o utils.o uart.o mm.o cutils.o dtp.o test.o syscall.o entry.o irq.o cirq.o csched.o sched.o syslib.o vfs.o tmpfs.o procfs.o main.o -T linker.ld -o kernel8.elf
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

dtp.o: dtp.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c dtp.c -o dtp.o

test.o: test.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c test.c -o test.o

syscall.o: syscall.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c syscall.c -o syscall.o

entry.o: entry.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c entry.S -o entry.o

irq.o: irq.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c irq.S -o irq.o

cirq.o: cirq.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c cirq.c -o cirq.o

csched.o: csched.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c csched.c -o csched.o

sched.o: sched.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c sched.S -o sched.o

syslib.o: syslib.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c syslib.S -o syslib.o

vfs.o: vfs.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c vfs.c -o vfs.o

tmpfs.o: tmpfs.c include/dtp.h
	aarch64-linux-gnu-gcc $(CFLAGS) -c tmpfs.c -o tmpfs.o

procfs.o: procfs.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c procfs.c -o procfs.o

main.o: main.c include/dtp.h
	aarch64-linux-gnu-gcc $(CFLAGS) -c main.c -o main.o

clean:
	rm kernel8.elf *.o >/dev/null 2>/dev/null || true
