# cross-compiler
COMPILER := aarch64-linux-gnu-gcc
LINKER := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy
QEMU := qemu-system-aarch64
ASM := a.S
OBJ := a.o
ELF := kernel8.elf
LD := linker.ld
IMG := kernel8.img
PROVIED_IMG := nctuos.img

all: lab00-compile lab00-exe


lab00-compile:
	$(COMPILER) -c $(ASM)
	$(LINKER) -T $(LD) -o $(ELF) $(OBJ)
	$(OBJCOPY) -O binary $(ELF) $(IMG)

lab00-exe:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -d in_asm

test:
	$(QEMU) -M raspi3 -kernel $(PROVIED_IMG) -display none -d in_asm

clean:
	rm -f $(OBJ) $(ELF) $(IMG)
