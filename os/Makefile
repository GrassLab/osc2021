# OSC 2021, check https://github.com/GrassLab/osc2021
include ../Makefile.inc

# set build parameters
LANG        ?= c # defualt set to C, use `LANG=rust make` to change implementation language
#ifeq ($(LANG), rust)
#	SRC_ROOT=rust-impl
#else
#	SRC_ROOT=c-impl
#endif

KERNEL_ROOT = kernel

LDFLAGS     += -L $(KERNEL_ROOT)/lib

IMAGE       = kernel8.img
ELF_FILE    = kernel8.elf
OBJ_FILES   = boot/boot.o $(KERNEL_OBJ) #$(addprefix $(KERNEL_ROOT)/, $(KERNEL_OBJ))

LINKER_SCRIPT=linker.lds

.PHONY: all clean img elf obj install


img: $(IMAGE)

include kernel/Makefile

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

clean: kercln
	rm -f $(IMAGE) $(ELF_FILE) $(OBJ_FILES)

