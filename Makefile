CC=clang
CFLAGS=-mcpu=cortex-a53 --target=aarch64-rpi3-elf -Wall -nostdinc -nostdlib
#CFLAGS+=-g -O0

#LD=aarch64-linux-gnu-ld
LD=ld.lld
LDFLAGS+=-m aarch64elf -nostdlib -T $(LINKER_SCRIPT)

OBJCPY=llvm-objcopy
OBJCPYFLAGS=--output-target=aarch64-rpi3-elf -O binary

IMAGE=kernel8.img
ELF_FILE=kernel8.elf
OBJ_FILES=boot.o kernel.o

LINKER_SCRIPT=linker.lds

.PHONY: all clean img elf obj

all: $(IMAGE)
	@echo ""
	@echo "==============================="
	@echo "$(IMAGE) image build completed!"

img: $(IMAGE)
$(IMAGE): %.img: %.elf
# Build kernel image from elf file
	$(OBJCPY) $(OBJCPYFLAGS) $< $@

elf: $(ELF_FILE)
$(ELF_FILE): $(OBJ_FILES)
# Linke elf file from object file
# TODO: Add $(LINKER_SCRIPT) as pre-request so that
# if it is modified, the target will re-run.
	$(LD) $(LDFLAGS) -o $@ $^

obj: $(OBJ_FILES)
%.o: %.S
# Compile from asm to obj file
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
# Compile from C to obj file
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(IMAGE) $(ELF_FILE) $(OBJ_FILES)
