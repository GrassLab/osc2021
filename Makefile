.PHONY: setup shell clean run debug gdb build

# target implementation to use
IMPL_FOLDER ?= impl-c
IS_BUILING_BOOT_LOADER = 0
# IMPL_FOLDER ?= bootloader
# IS_BUILING_BOOT_LOADER = 1

QEMU = qemu-system-aarch64
CROSS_GDB = aarch64-linux-gdb

KERNEL_IMG = kernel8.img
KERNEL_ELF = kernel8.elf

INIT_RAM_FS = res/initramfs.cpio
INIT_RAM_FS_SRC = res/rootfs

IMPL_KERNEL_IMG = $(IMPL_FOLDER)/$(KERNEL_IMG)
IMPL_KERNEL_ELF = $(IMPL_FOLDER)/$(KERNEL_ELF)

# change default img to run/debug
ifeq ($(IS_BUILING_BOOT_LOADER),1)
IMPL_FOLDER = bootloader
IMPL_KERNEL_IMG = bootloader/bootloader.img
IMPL_KERNEL_ELF = bootloader/bootloader.elf
endif

# select which folder need to run under venv
IMPL_TO_USE_VENV:= bootloader impl-c
ifneq ($(filter $(IMPL_FOLDER),$(IMPL_TO_USE_VENV)),)
    USE_VENV = 1
else
    USE_VENV = 0
endif

ifeq ($(USE_VENV),1)
    # C impl settings
    DOCKCROSS_SCRIPT = dockcross-linux-aarch64
    ENV_RUN=./$(DOCKCROSS_SCRIPT)
    all buil clean shell: $(DOCKCROSS_SCRIPT)
else
    # Rust impl settings
    ENV_RUN=
endif

all: build $(INIT_RAM_FS)
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
ifeq ($(IMPL_FOLDER),impl-rs)
	@echo "${YELLOW} ❌ Build under rust impl, no virtual env needed${RESET}"
else
	@echo "${YELLOW} 🤖 Spawn shell inside ${DOCKER} ${RESET}"
	$(ENV_RUN) bash
endif

clean:
	$(ENV_RUN) make -C $(IMPL_FOLDER) clean
	$(RM) $(INIT_RAM_FS)
	@echo "${YELLOW} 🚚 Finish cleanup${RESET}"

run: all
	@echo "${YELLOW} 🚧 Run kernel with QEMU${RESET}"
	$(RUN_WITH_KERNEL) $(IMPL_KERNEL_IMG)

debug: all
	@echo "${YELLOW} 🐛 Start debugging${RESET}"
	@echo "${YELLOW}Open another terminal and use ${GREEN}make gdb${YELLOW} to connect to host${RESET}"
	$(DEBUG_SERVER_WITH_KERNEL) $(IMPL_KERNEL_IMG)

gdb: all
	@echo "${YELLOW} 🕵️‍♀️ Using ${GREEN}${CROSS_GDB}${RESET}"
	$(CROSS_GDB) --init-command osc-gdb

# == Resources

$(INIT_RAM_FS):  $(shell find $(INIT_RAM_FS_SRC))
	@echo "${GREEN} 📦 Generate ramfs file from ${YELLOW}$(INIT_RAM_FS_SRC)${GREEN} to ${YELLOW}${INIT_RAM_FS}${RESET}"
	cd $(INIT_RAM_FS_SRC) && find . |  cpio -o -H newc> ../$(@F)

# ==

define DEBUG_SERVER_WITH_KERNEL
	$(QEMU) -M raspi3 \
	-chardev pty,signal=on,id=char0 \
	-serial null -serial chardev:char0 \
	-display none \
	-initrd $(INIT_RAM_FS) \
	-s -S \
	-kernel
endef

# define RUN_WITH_KERNEL
# 	@echo "${GREEN} 📬 Multiplexing qemu pty device${RESET}"
# 	$(QEMU) -M raspi3 \
# 	-chardev pty,signal=on,id=char0 \
# 	-serial null -serial chardev:char0 \
# 	-display none \
# 	-initrd $(INIT_RAM_FS) \
# 	-kernel
# endef

define RUN_WITH_KERNEL
	@echo "${GREEN} 📬 Multiplexing qemu pty device${RESET}"
	$(QEMU) -M raspi3 \
	-serial null -serial stdio \
	-display none \
	-initrd $(INIT_RAM_FS) \
	-kernel
endef
