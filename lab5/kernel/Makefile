CC       := aarch64-linux-gnu-gcc
CXX      := aarch64-linux-gnu-g++
LD       := aarch64-linux-gnu-ld
OBJCOPY  := aarch64-linux-gnu-objcopy
CCFLAGS  := -std=gnu17
CXXFLAGS := -ffreestanding -nostdinc -nostdlib -nostdinc++ -nostartfiles -g -std=c++17 -MMD -Iinclude
SRC      := $(wildcard *.cpp)
ASM      := $(wildcard *.S)
OBJ      := $(SRC:.cpp=.o) $(ASM:.S=.o)
DEP      := $(SRC:.cpp=.d)
LD_SCRIPT:= kernel8.ld
OUTPUT   := kernel8.elf

.PHONY: all clean
all: kernel8.elf

%.o: %.S
	$(CC) $(CCFLAGS) -c $< -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(OUTPUT): $(LD_SCRIPT) $(OBJ)
	$(LD) -T $< -o $@ $(filter-out $<, $^)
clean:
	rm -rf *.o *.d $(OUTPUT)
-include $(DEP)
