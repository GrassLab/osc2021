.PHONY:all run clean onboard
APP:=kernel8
LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles

all:
	cd inc&&make
	make $(APP).img

$(APP).img:start.o main.o inc/uart.o
	aarch64-linux-gnu-ld -T $(LINKER) -o $(APP).elf start.o main.o inc/uart.o
	aarch64-linux-gnu-objcopy -O binary $(APP).elf $@
	rm $(APP).elf

start.o:start.S
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<

%.o:%.c
	aarch64-linux-gnu-gcc $(CFLAGS) -c $<

run:
	make all
	qemu-system-aarch64 -M raspi3 -kernel $(APP).img -display none -serial null -serial stdio

onboard:
	sudo screen /dev/ttyUSB0 115200

clean:
	cd inc&&make clean
	-rm $(APP).img
	-rm *.o
