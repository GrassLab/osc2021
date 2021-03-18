include ../Makefile.inc

CFLAGS      += -I include

LDFLAGS     += -L lib

IMAGE       = bootloader.img
ELF_FILE    = bootloader.elf
OBJ_FILES   = boot.o bootloader.o lib/util.o lib/shell.o lib/mmio.o

LINKER_SCRIPT=linker.lds


.PHONY: all clean img elf obj install


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
