# OSC 2021, check https://github.com/GrassLab/osc2021

# set build parameters
ENV         ?= release # default set to release mode, use `ENV=debug make` to change to debug mode

CC          = clang
CFLAGS      = -mcpu=cortex-a53 --target=aarch64-rpi3-elf -Wall -nostdinc -nostdlib
ifeq ($(ENV), debug)
	CFLAGS+=-g -O0 -D_QEMU
endif

LD          = ld.lld
LDFLAGS     = -m aarch64elf -nostdlib

OBJCPY      = llvm-objcopy
OBJCPYFLAGS = --output-target=aarch64-rpi3-elf -O binary
