TOOLCHAIN_PREFIX ?= aarch64-unknown-linux-gnueabi
CC := $(TOOLCHAIN_PREFIX)-gcc
LD:= $(TOOLCHAIN_PREFIX)-ld
OBJCOPY := $(TOOLCHAIN_PREFIX)-objcopy

CFLAGS = -Wall -nostartfiles -ffreestanding


HELLO = hello_world.out
GETPID = get_pid.out
ARGV = argv_test.out
all: $(HELLO) $(GETPID) $(ARGV)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

libs = lib.o stdio.o

HELLO_DEPS = $(libs) hello_world.o
HELLO_ELF = hello_world.elf
$(HELLO): $(HELLO_DEPS)
	$(LD) $(HELLO_DEPS) -T linker.ld -o $(HELLO_ELF)
	$(OBJCOPY) -O binary $(HELLO_ELF) $(HELLO)

GETPID_DEPS = $(libs) get_pid.o
GETPID_ELF = get_pid.elf
$(GETPID): $(GETPID_DEPS)
	$(LD) $(GETPID_DEPS) -T linker.ld -o $(GETPID_ELF)
	$(OBJCOPY) -O binary $(GETPID_ELF) $(GETPID)

ARGV_DEPS = $(libs) argv_test.o
ARGV_ELF = argv_test.elf
$(ARGV): $(ARGV_DEPS)
	$(LD) $(ARGV_DEPS) -T linker.ld -o $(ARGV_ELF)
	$(OBJCOPY) -O binary $(ARGV_ELF) $(ARGV)


.PHONY: clean
clean:
	$(RM) -f *.o *.elf *.out
