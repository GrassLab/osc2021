CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
LD_SRC   := linker.ld
CCFLAGS  := -std=gnu17
CXXFLAGS := -Iinclude -ffreestanding -nostdinc -nostdlib -nostartfiles -std=gnu++14 -g

INCLUDE  := $(shell find * -name '*.h')
SRC      := $(shell find * -name '*.cpp')
ASM      := $(shell find * -name '*.S')
OBJ      := $(patsubst %.cpp,%.o,$(SRC)) $(patsubst %.S,%.o,$(ASM))

.PHONY: all clean qemu-debug
all: kernel8.img
clean:
	find \( -name '*.o' -o -name '*.img' -o -name '*.elf' \) -delete
%.o: %.S
	$(CC) $(CCFLAGS) -c $< -o $@
%.o: %.cpp $(INCLUDE)
	$(CXX) $(CXXFLAGS) -c $< -o $@
kernel8.elf: $(LD_SRC) $(filter src/bootloader/%, $(OBJ))
	$(LD) -T $(LD_SRC) -o $@ $(filter-out $(LD_SRC), $^)
kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@
qemu-debug: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -serial null -serial stdio -S -s
