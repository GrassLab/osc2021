ARM = aarch64-linux-gnu
CC = $(ARM)-gcc
LD = $(ARM)-ld
OBJCOPY = $(ARM)-objcopy


CFLAGS = -Wall -g -ffreestanding -march=armv8-a

SRCS = $(wildcard *.c)
IMGS = $(SRCS:%.c=%.img)

LINKER = linker.ld

all: $(IMGS)

%.img: %.elf
	$(OBJCOPY) -O binary $< $@

%.elf: %.o start.asmo
	$(LD) -T $(LINKER) --gc-sections -g -o $@ $< start.asmo

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

start.asmo: start.S
	$(CC) $(CFLAGS) -c start.S -o start.asmo

clean:
	rm -f start.asmo *.img