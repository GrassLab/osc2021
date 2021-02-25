ARCH_PRE := aarch64-unknown-linux-gnueabi-

CC := $(ARCH_PRE)gcc
LD:= $(ARCH_PRE)ld
OBJCOPY := $(ARCH_PRE)objcopy

CFLAGS = -Wall -nostdlib

ASM = $(wildcard *.S)

OBJS = $(ASM:%.S=build/%_s.o)

TARGET = kernel8.img
TARGET_ELF = kernel8.elf

all: $(TARGET)

build/%_s.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(TARGET_ELF)
	@mkdir -p $(@D)
	$(OBJCOPY) -O binary $< $@

$(TARGET_ELF): $(OBJS)
	$(LD) -T linker.ld -o $@ $(OBJS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(TARGET_ELF) $(OBJS)