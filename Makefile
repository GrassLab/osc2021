BUILD=debug

CC=clang
CFLAGS=-mcpu=cortex-a53 --target=aarch64-rpi3-elf -Wall
CFLAGS+=-g -O0

LD=aarch64-linux-gnu-ld
LDFLAGS+=-T $(LINKER_SCRIPT)

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
	llvm-objcopy -O binary $< $@

elf: $(ELF_FILE)
$(ELF_FILE): $(OBJ_FILES)
# Linke elf file from object file
	$(LD) $(LDFLAGS) -o $@ $?

obj: $(OBJ_FILES)
%.o: %.S
# Compile from asm to obj file
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
# Compile from C to obj file
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(IMAGE) $(ELF_FILE) $(OBJ_FILES)
