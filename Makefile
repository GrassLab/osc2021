# cross-compiler
COMPILER := aarch64-linux-gnu-gcc
LINKER := aarch64-linux-gnu-ld
OBJCOPY := aarch64-linux-gnu-objcopy
QEMU := qemu-system-aarch64
START := head
OBJ := main.o uart.o string.o reboot.o
ELF := kernel8.elf
LD := linker.ld
IMG := kernel8.img
PROVIED_IMG := nctuos.img
GDB := gdb-multiarch
CCFLAG := -Wall -nostdlib -Og
#MINI-UART := -serial null -serial stdio
MINI-UART :=

all: $(OBJ) compile

%.o: %.c
	$(COMPILER) -c $< -o $@ $(CCFLAG)

compile:
	$(COMPILER) -c $(START).S -o $(START).o
	$(LINKER) -T $(LD) -o $(ELF) $(START).o $(OBJ)
	$(OBJCOPY) -O binary $(ELF) $(IMG)

exe:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -serial null -serial stdio

dump:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -d in_asm

test:
	$(QEMU) -M raspi3 -kernel $(PROVIED_IMG) -display none -d in_asm

debug:
	$(QEMU) -M raspi3 -kernel $(IMG) -display none -S -s $(MINI-UART)

gdb:
	@echo "target remote :1234"
	$(GDB) $(IMG)


clean:
	rm -f $(START).o $(ELF) $(IMG) $(OBJ)
