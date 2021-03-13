.PHONY: shell clean

kernel_impl = impl-c
# kernel_impl = impl-rs
bootloader_impl = bootloader

INIT_RAM_FS = res/initramfs.cpio
INIT_RAM_FS_SRC = res/rootfs
KERNEL_IMG = res/kernel/kernel8.img
BOOT_LOADER_IMG = res/bootloader/kernel8.img

QEMU = qemu-system-aarch64
CROSS_GDB = aarch64-linux-gdb
CROSS = ./dockcross-linux-aarch64

# define standard colors
RED          := $(shell tput setaf 1)
GREEN        := $(shell tput setaf 2)
YELLOW       := $(shell tput setaf 3)
WHITE        := $(shell tput setaf 7)
RESET := $(shell tput sgr0)

# == Commands
all: $(KERNEL_IMG) $(BOOT_LOADER_IMG) $(INIT_RAM_FS)
	@echo "${YELLOW} 📦 Build Finished${RESET}"

shell:
	@echo "${YELLOW} 🤖 Spawn shell inside docker ${RESET}"
	$(CROSS) bash

clean:
	@python3 scripts/builder.py $(kernel_impl) clean
	@python3 scripts/builder.py $(bootloader_impl) clean
	$(RM) $(INIT_RAM_FS)
	$(RM) -rf scripts/__pycache__
	@echo "${YELLOW} 🚚 Finish cleanup${RESET}"

.PHONY: export
export:
	poetry export -f requirements.txt --output requirements.txt

.PHONY: bootloader-stdio
bootloader-stdio: all
	@echo "${YELLOW} 🚧 Start bootloader(stdio)${RESET}"
	@echo "${YELLOW} Use ${RED}ctrl-c${YELLOW} to quit session${RESET}"
	$(RUN_STDIO_KERNEL) $(BOOT_LOADER_IMG)

.PHONY: bootloader-tty
bootloader-tty: all
	@echo "${YELLOW} 🚧 Start bootloader(tty)${RESET}"
	@echo "${YELLOW} Use ${RED}ctrl-c${YELLOW} to quit session${RESET}"
	@$(RUN_TTY_KERNEL) $(BOOT_LOADER_IMG)

.PHONY: run-stdio
run-stdio: all
	@echo "${YELLOW} 🚧 Start kernel(stdio)${RESET}"
	@echo "${YELLOW} Use ${RED}ctrl-c${YELLOW} to quit session${RESET}"
	@$(RUN_STDIO_KERNEL) $(KERNEL_IMG)

.PHONY: run-tty
run-tty: all
	@echo "${YELLOW} 🚧 Start kernel(tty)${RESET}"
	@echo "${YELLOW} Use ${RED}ctrl-c${YELLOW} to quit session${RESET}"
	@$(RUN_TTY_KERNEL) $(KERNEL_IMG)

.PHONY: debug
debug: all
	@echo "${YELLOW} 🐛 Start debugging kernel${RESET}"
	@echo "${YELLOW}Open another terminal and use ${GREEN}make gdb${YELLOW} to connect to host${RESET}"
	@$(DEBUG_KERNEL) $(KERNEL_IMG)

.PHONY: debug-bootloader
debug-bootloader: all
	@echo "${YELLOW} 🐛 Start debugging bootloader${RESET}"
	@echo "${YELLOW}Open another terminal and use ${GREEN}make gdb-bootloader${YELLOW} to connect to host${RESET}"
	@$(DEBUG_KERNEL) $(BOOT_LOADER_IMG)

.PHONY: gdb
gdb: all
	@echo "${YELLOW} 🕵️‍♀️ Using ${GREEN}${CROSS_GDB}${RESET}"
	$(CROSS_GDB) --init-command $(kernel_impl)/gdbinit

.PHONY: gdb-bootloader
gdb-bootloader: all
	@echo "${YELLOW} 🕵️‍♀️ Using ${GREEN}${CROSS_GDB}${RESET}"
	$(CROSS_GDB) --init-command $(bootloader_impl)/gdbinit

# == Resources
$(BOOT_LOADER_IMG): $(shell find $(bootloader_impl))
	@python3 scripts/builder.py $(bootloader_impl) build

$(KERNEL_IMG): $(shell find $(kernel_impl))
	@python3 scripts/builder.py $(kernel_impl) build

$(INIT_RAM_FS):  $(shell find $(INIT_RAM_FS_SRC))
	@echo "${GREEN} 📦 Generate ramfs file from ${YELLOW}$(INIT_RAM_FS_SRC)${GREEN} to ${YELLOW}${INIT_RAM_FS}${RESET}"
	cd $(INIT_RAM_FS_SRC) && find . |  cpio -o -H newc> ../$(@F)

# ==

# (Not called directly)
# Start qemu
define _run_qemu_base
	$(QEMU) -M raspi3 \
	-initrd $(INIT_RAM_FS) \
	-display none
endef

# (Not called directly)
# Start qemu with creating multiplexed tty port on host
define _run_qemu_mux_base
	$(_run_qemu_base) \
	-chardev pty,signal=on,id=char_mux \
	-serial null -serial chardev:char_mux
endef

# Start qemu with stdio attached
define RUN_STDIO_KERNEL
	$(_run_qemu_base) \
	-serial null -serial stdio \
	-kernel
endef

# Start qemu with tty opend on host
define RUN_TTY_KERNEL
	$(_run_qemu_mux_base) \
	-kernel
endef

# Start qemu with tty opend on host, waiting for gdb to connect
define DEBUG_KERNEL
	$(_run_qemu_mux_base) \
	-s -S \
	-kernel
endef
