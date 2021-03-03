CROSS_COMPILE ?= aarch64-linux-gnu-

arch := $(shell uname -m)
ifeq ($(arch), arm64)
	undefine CROSS_COMPILE
endif

ifeq ($(arch), aarch64)
	undefine CROSS_COMPILE
endif

CC := $(CROSS_COMPILE)gcc
CFLAGS = -mcpu=cortex-a53 \
	-fno-common -ffreestanding -O0 \
	-Wall -Werror \
	-Wl,-Tlinker.ld -nostartfiles

TARGET = kernel8.img
SRC_FILES = startup.S main.c uart.c

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o kernel8.elf
	$(CROSS_COMPILE)objcopy -O binary kernel8.elf kernel8.img

run: $(TARGET)
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio

clean:
	rm -f *.elf *.img *.o
