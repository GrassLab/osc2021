include ../Makefile.inc

SRC_PATH    = src
INC_PATH    = include
LIB_PATH    = lib

SRC_C       = $(wildcard $(SRC_PATH)/*.c)

CFLAGS      += -I $(INC_PATH)

LDFLAGS     += -L $(LIB_PATH)

IMAGE       = bootloader.img
ELF_FILE    = bootloader.elf
OBJ_FILES   = $(LIB_PATH)/boot.o \
              $(LIB_PATH)/bootloader.o \
              $(patsubst $(SRC_PATH)/%, $(LIB_PATH)/%, ${SRC_C:.c=.o})

ifeq ($(ENV), debug)
LINKER_SCRIPT=linker_qemu.lds
else
LINKER_SCRIPT=linker.lds
endif

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
$(LIB_PATH)/%.o: $(SRC_PATH)/%.S
# Compile from asm to obj file
	mkdir -p $(LIB_PATH)
	$(CC) $(CFLAGS) -c $< -o $@
$(LIB_PATH)/%.o: $(SRC_PATH)/%.c
# Compile from C to obj file
	mkdir -p $(LIB_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(LIB_PATH)
	rm -f $(IMAGE) $(ELF_FILE)
