SRC       := $(shell find . -name '*.cpp')
ASM       := $(shell find . -name '*.S')
OBJ       := $(SRC:.cpp=.o) $(ASM:.S=.o)
DEP       := $(SRC:.cpp=.d)
LD_SCRIPT := program.ld
PROGS     := $(filter-out ./lib ./include, $(shell find . -maxdepth 1 -mindepth 1 -type d))
PROGS_ELF := $(patsubst %,%.elf,$(PROGS))
PERECENT_ := %

.PHONY: all clean
.PRECIOUS: %.o
all: $(PROGS_ELF)

%.o: %.S
	$(CC) $(CCFLAGS) -c $< -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	find . \( -name '*.o' -o -name '*.d' \) -delete
	rm -rf $(PROGS_ELF)
-include $(DEP)
.SECONDEXPANSION:
%.elf: $(LD_SCRIPT) $(filter ./lib/%,$(OBJ)) $$(filter ./%/$$(PERECENT_),$$(OBJ))
	$(LD) -T $< -o $@ $(filter-out $<, $^)
