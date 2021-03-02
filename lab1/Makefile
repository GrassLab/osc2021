CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
LD_SRC   := linker.ld
CCFLAGS  := -std=gnu17
CXXFLAGS := -Iinclude -ffreestanding -nostdinc -nostdlib -nostartfiles -std=gnu++14 -g

INCLUDE  := $(wildcard **/*.h)
SRC      := $(wildcard **/*.cpp)
ASM      := $(wildcard **/*.S)
OBJ      := $(patsubst %.cpp,%.o,$(SRC)) $(patsubst %.S,%.o,$(ASM))

.PHONY: all clean
all: kernel8.img
clean:
	rm -rf **/*.o *.elf *.img
%.o: %.S
	$(CC) $(CCFLAGS) -c $< -o $@
%.o: %.cpp $(INCLUDE)
	$(CXX) $(CXXFLAGS) -c $< -o $@
kernel8.elf: $(LD_SRC) $(OBJ)
	$(LD) -T $(LD_SRC) -o $@ $(filter-out $(LD_SRC), $^)
kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@
