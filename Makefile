.PHONY: setup shell clean run debug gdb build

# target implementation to use
IMPL_FOLDER ?= impl-rs

QEMU = qemu-system-aarch64
CROSS_GDB = aarch64-linux-gdb

KERNEL_IMG = kernel8.img
KERNEL_ELF = kernel8.elf

IMPL_KERNEL_IMG = $(IMPL_FOLDER)/$(KERNEL_IMG)
IMPL_KERNEL_ELF = $(IMPL_FOLDER)/$(KERNEL_ELF)

ifeq ($(IMPL_FOLDER),impl-c)
    # C impl settings
    DOCKCROSS_SCRIPT = dockcross-linux-aarch64
    ENV_RUN=./$(DOCKCROSS_SCRIPT)
    all buil clean shell: $(DOCKCROSS_SCRIPT)
else
    # Rust impl settings
    ENV_RUN=
endif

all: build
	@echo "${YELLOW} 📦 Build Finished${RESET}"

build:
	$(ENV_RUN) make -C $(IMPL_FOLDER)

# define standard colors
RED          := $(shell tput setaf 1)
GREEN        := $(shell tput setaf 2)
YELLOW       := $(shell tput setaf 3)
WHITE        := $(shell tput setaf 7)
RESET := $(shell tput sgr0)

# == Commands

shell:
ifneq ($(IMPL_FOLDER),impl-rs)
	@echo "${YELLOW} ❌ Building for rust, no virtual env needed${RESET}"
else
	@echo "${YELLOW} 🤖 Spawn shell inside ${DOCKER} ${RESET}"
	$(ENV_RUN) bash
endif

clean:
	$(ENV_RUN) make -C $(IMPL_FOLDER) clean
	@echo "${YELLOW} 🚚 Finish cleanup${RESET}"

run: all
	@echo "${YELLOW} 🚧 Run kernel with QEMU${RESET}"
	$(RUN_WITH_KERNEL) $(IMPL_KERNEL_IMG) -serial null -serial stdio

debug: all
	@echo "${YELLOW} 🐛 Start debugging${RESET}"
	@echo "${YELLOW}Open another terminal and use ${GREEN}make gdb${YELLOW} to connect to host${RESET}"
	$(DEBUG_SERVER_WITH_KERNEL) $(IMPL_KERNEL_IMG) -serial null -serial stdio

gdb: all
	@echo "${YELLOW} 🕵️‍♀️ Using ${GREEN}${CROSS_GDB}${RESET}"
	$(CROSS_GDB) --init-command osc-gdb
# ==

define DEBUG_SERVER_WITH_KERNEL
	$(QEMU) -M raspi3 \
	-display none \
	-s -S \
	-kernel
endef

define RUN_WITH_KERNEL
	$(QEMU) -M raspi3 \
	-display none \
	-kernel
endef
