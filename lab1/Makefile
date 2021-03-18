SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = kernel8.img
ELF_TARGET = kernel8.elf

CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-ld
OC = aarch64-linux-gnu-objcopy
QEMU = qemu-system-aarch64

CFLAGS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib

.PHONY: all clean run debug 

all: clean $(TARGET)

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): start.o $(OBJS)
	$(LD) $(LDFLAGS) start.o $(OBJS) -T linker.ld -o $(ELF_TARGET)
	$(OC) -O binary $(ELF_TARGET) $(TARGET)

clean:
	- rm *.o $(ELF_TARGET) $(TARGET) > /dev/null 2> /dev/null

run:
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial null -serial stdio --display none

debug:
	$(QEMU) -M raspi3 -kernel $(TARGET) -serial null -serial stdio -S -s
