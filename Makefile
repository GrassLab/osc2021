QEMU = qemu-system-aarch64
CROSS_GDB = aarch64-linux-gdb
DOCKER = ./dockcross-linux-arm64

.PHONY: shell clean run debug gdb

SRC_FOLDER = osc

KERNEL_IMG = kernel8.img
KERNEL_ELF = kernel8.elf

all: $(SRC_FOLDER)/$(KERNEL_IMG) $(SRC_FOLDER)/$(KERNEL_ELF)
	@echo "${YELLOW} 📦 Build Finished${RESET}"


# define standard colors
RED          := $(shell tput setaf 1)
GREEN        := $(shell tput setaf 2)
YELLOW       := $(shell tput setaf 3)
WHITE        := $(shell tput setaf 7)
RESET := $(shell tput sgr0)

# == Commands
shell:
	@echo "${YELLOW} 🤖 Spawn shell inside ${DOCKER} ${RESET}"
	$(DOCKER) bash

clean:
	$(DOCKER) make -C $(SRC_FOLDER) clean
	@echo "${YELLOW} 🚚 Finish cleanup${RESET}"

run: all
	@echo "${YELLOW} 🚧 Run kernel with QEMU${RESET}"
	$(RUN_WITH_KERNEL) $(SRC_FOLDER)/$(KERNEL_IMG) -serial null -serial stdio

debug: all
	@echo "${YELLOW} 🐛 Start debugging${RESET}"
	@echo "${YELLOW}Open another terminal and use ${GREEN}make gdb${YELLOW} to connect to host${RESET}"
	$(DEBUG_SERVER_WITH_KERNEL) $(SRC_FOLDER)/$(KERNEL_IMG)

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

 $(SRC_FOLDER)/$(KERNEL_IMG): FORCE
	$(DOCKER) make -C $(SRC_FOLDER)

 $(SRC_FOLDER)/$(KERNEL_ELF): FORCE
	$(DOCKER) make -C $(SRC_FOLDER)

FORCE: