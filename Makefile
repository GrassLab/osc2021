# OSC 2021, check https://github.com/GrassLab/osc2021

# set build parameters
LANG ?= c # defualt set to C, use `LANG=rust make` to change implementation language
ENV ?= release # default set to release mode, use `ENV=debug make` to change to debug mode
SD_MEDIA ?= 4DFF-0A36

ifeq ($(LANG), rust)
	SRC_ROOT=rust-impl
else
	SRC_ROOT=c-impl
endif

CC=clang
CFLAGS=-mcpu=cortex-a53 --target=aarch64-rpi3-elf -Wall -nostdinc -nostdlib -I $(SRC_ROOT)/include
ifeq ($(ENV), debug)
	CFLAGS+=-g -O0
endif

#LD=aarch64-linux-gnu-ld
LD=ld.lld
LDFLAGS+=-m aarch64elf -nostdlib -L $(SRC_ROOT)/lib

OBJCPY=llvm-objcopy
OBJCPYFLAGS=--output-target=aarch64-rpi3-elf -O binary

IMAGE=kernel8.img
ELF_FILE=kernel8.elf
OBJ_FILES=boot/boot.o $(SRC_ROOT)/kernel.o $(SRC_ROOT)/lib/util.o $(SRC_ROOT)/lib/shell.o $(SRC_ROOT)/lib/mmio.o

LINKER_SCRIPT=linker.lds

.PHONY: all clean img elf obj install

all: $(IMAGE)
	@echo ""
	@echo "==============================="
	@echo "$(IMAGE) image build completed!"

img: $(IMAGE)
$(IMAGE): %.img: %.elf
# Build kernel image from elf file
	$(OBJCPY) $(OBJCPYFLAGS) $< $@

elf: $(ELF_FILE)
$(ELF_FILE): $(OBJ_FILES) $(LINKER_SCRIPT)
# Linke elf file from object file
	$(LD) $(LDFLAGS) -T $(LINKER_SCRIPT) -o $@ $(filter-out $(LINKER_SCRIPT), $^)

obj: $(OBJ_FILES)
%.o: %.S
# Compile from asm to obj file
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
# Compile from C to obj file
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(IMAGE) $(ELF_FILE) $(OBJ_FILES)

install:
#cp ./setup/* /run/media/calee/$(SD_MEDIA)
	cp ./kernel8.img /run/media/calee/$(SD_MEDIA)
