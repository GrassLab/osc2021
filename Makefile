QEMU = qemu-system-aarch64
CROSS_GDB = aarch64-linux-gdb

CROSS_CONTAINER_IMG = dockcross-linux-aarch64
DOCKCROSS_SCRIPT = $(CROSS_CONTAINER_IMG)
CROSS_ENV_RUN = ./$(DOCKCROSS_SCRIPT)

.PHONY: setup shell clean run debug gdb build

# target implementation to use
IMPL_FOLDER ?= impl-rs

KERNEL_IMG = kernel8.img
KERNEL_ELF = kernel8.elf

IMPL_KERNEL_IMG = $(IMPL_FOLDER)/$(KERNEL_IMG)
IMPL_KERNEL_ELF = $(IMPL_FOLDER)/$(KERNEL_ELF)

all: $(DOCKCROSS_SCRIPT) build
	@echo "${YELLOW} 📦 Build Finished${RESET}"

build: $(DOCKCROSS_SCRIPT)
	$(CROSS_ENV_RUN) make -C $(IMPL_FOLDER)

# define standard colors
RED          := $(shell tput setaf 1)
GREEN        := $(shell tput setaf 2)
YELLOW       := $(shell tput setaf 3)
WHITE        := $(shell tput setaf 7)
RESET := $(shell tput sgr0)

# == Commands
setup:
	$(RUN_BUILD_DOCKER_IMAGE)
	docker run $(CROSS_CONTAINER_IMG) > $(DOCKCROSS_SCRIPT)
	chmod +x $(DOCKCROSS_SCRIPT)
	@echo "${YELLOW} 🎉 You're all set 🎉 ${RESET}"

shell: $(DOCKCROSS_SCRIPT)
	@echo "${YELLOW} 🤖 Spawn shell inside ${DOCKER} ${RESET}"
	$(CROSS_ENV_RUN) bash

clean: $(DOCKCROSS_SCRIPT)
	$(CROSS_ENV_RUN) make -C $(IMPL_FOLDER) clean
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

define RUN_BUILD_DOCKER_IMAGE
	@echo "${YELLOW} 👷‍♂️ Start Building Cross compiling env with Docker${RESET}"
	docker build -t $(CROSS_CONTAINER_IMG) .
	@echo "${YELLOW} ✅ Cross compiling env built${RESET}"
	@echo "${GREEN}"
	docker images ${CROSS_CONTAINER_IMG}
	@echo "${RESET}"
endef

$(DOCKCROSS_SCRIPT): Dockerfile
	$(RUN_BUILD_DOCKER_IMAGE)
	docker run $(CROSS_CONTAINER_IMG) > $(DOCKCROSS_SCRIPT)
	chmod +x $(DOCKCROSS_SCRIPT)

