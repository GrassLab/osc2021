TOOLCHAIN_PREFIX ?= aarch64-unknown-linux-gnueabi
CC := $(TOOLCHAIN_PREFIX)-gcc
LD:= $(TOOLCHAIN_PREFIX)-ld
OBJCOPY := $(TOOLCHAIN_PREFIX)-objcopy

CFLAGS = -Wall -nostartfiles -ffreestanding


NAME= user_program
SRC = $(NAME).S
OBJ = $(NAME).o
ELF = $(NAME).elf
TARGET = $(NAME).out


$(TARGET):
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)
	$(LD) $(OBJ) -T linker.ld -o $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(TARGET)

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJ) $(ELF)
