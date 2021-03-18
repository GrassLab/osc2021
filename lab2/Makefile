CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
CCFLAGS  := -std=gnu17
CXXFLAGS := -Iinclude -ffreestanding -nostdinc -nostdlib -nostartfiles -std=gnu++17 -g -O3 -MMD

#INCLUDE  := $(shell find * -name '*.h')
SRC      := $(shell find * -name '*.cpp')
ASM      := $(shell find * -name '*.S')
OBJ      := $(patsubst src/%.cpp,lib/%.o,$(SRC)) $(patsubst src/%.S,lib/%.o,$(ASM))
DEP      := $(patsubst %.o,%.d,$(OBJ))

MAKEFLAGS += --no-builtin-rules --no-builtin-variables

.PHONY: all clean qemu-debug
all: kernel8.img terminal.img
clean:
	rm -rf lib *.elf *.img
lib/%.o: src/%.S
	@mkdir -p $(@D)
	$(CC) $(CCFLAGS) -c $< -o $@
lib/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@
bootloader.elf: bootloader.ld $(filter lib/bootloader/%, $(OBJ))
	$(LD) -T $< -o $@ $(filter-out $<, $^)
terminal.elf: terminal.ld $(filter lib/terminal/%, $(OBJ))
	$(LD) -T $< -o $@ $(filter-out $<, $^)
kernel8.img: bootloader.elf
	$(OBJCOPY) -O binary $< $@
terminal.img: terminal.elf
	$(OBJCOPY) -O binary $< $@
qemu-debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial pty -S -s

-include $(DEP)
