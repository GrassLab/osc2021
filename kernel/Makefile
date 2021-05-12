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
