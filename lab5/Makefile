CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
CCFLAGS  := -std=gnu17
CXXFLAGS := -Iinclude -ffreestanding -nostdinc -nostdlib -nostdinc++ -nostartfiles -g -std=c++17 -fno-tree-loop-distribute-patterns
INCLUDE  := $(shell find * -name '*.h')
SRC      := $(shell find * -name '*.cpp')
ASM      := $(shell find * -name '*.S')
OBJ      := $(patsubst %.cpp,%.o,$(SRC)) $(patsubst %.S,%.o,$(ASM))


.PHONY: all clean qemu-debug
all: terminal.img
clean:
	find \( -name '*.o' -o -name '*.img' -o -name '*.elf' \) -delete
%.o: %.S
	$(CC) $(CCFLAGS) -c $< -o $@
%.o: %.cpp $(INCLUDE)
	$(CXX) $(CXXFLAGS) -c $< -o $@
terminal.elf: terminal.ld $(filter src/terminal/%, $(OBJ))
	$(LD) -T $< -o $@ $(filter-out $<, $^)
terminal.img: terminal.elf
	$(OBJCOPY) -O binary $< $@
qemu-debug: all
	qemu-system-aarch64 -M raspi3 -kernel terminal.img -display none -serial null -serial stdio -S -s
run: all
	qemu-system-aarch64 -M raspi3 -kernel terminal.img -display none -serial null -serial stdio

