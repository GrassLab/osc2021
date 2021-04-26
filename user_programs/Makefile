TOOLCHAIN_PREFIX ?= aarch64-unknown-linux-gnueabi
CC := $(TOOLCHAIN_PREFIX)-gcc
LD:= $(TOOLCHAIN_PREFIX)-ld
OBJCOPY := $(TOOLCHAIN_PREFIX)-objcopy

CFLAGS = -Wall -nostartfiles -ffreestanding -ffunction-sections


HELLO = hello_world.out
GETPID = get_pid.out
all: $(HELLO) $(GETPID)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

HELLO_DEPS = lib.o hello_world.o
HELLO_ELF = hello_world.elf
hello_world.out: $(HELLO_DEPS)
	$(LD) $(HELLO_DEPS) -T linker.ld -o $(HELLO_ELF)
	$(OBJCOPY) -O binary $(HELLO_ELF) $(HELLO)


GETPID_DEPS = lib.o get_pid.o
GETPID_ELF = get_pid.elf
get_pid.out: $(GETPID_DEPS)
	$(LD) $(GETPID_DEPS) -T linker.ld -o $(GETPID_ELF)
	$(OBJCOPY) -O binary $(GETPID_ELF) $(GETPID)


.PHONY: clean
clean:
	$(RM) -f *.o *.elf *.out
