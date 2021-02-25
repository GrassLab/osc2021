QEMU = qemu-system-aarch64
DOCKER = ./dockcross-linux-arm64
KERNEL_IMG = kernel8.img

.PHONY: shell clean run

all: $(KERNEL_IMG)

# == Commands
run: $(KERNEL_IMG)
	$(QEMU_WITH_KERNEL) $<

shell:
	$(DOCKER) bash

clean:
	$(RM) $(KERNEL_IMG)
	$(DOCKER) make -C ./src clean

define QEMU_WITH_KERNEL
	$(QEMU) -M raspi3 \
	-display none \
	-d in_asm\
	-kernel
endef

$(KERNEL_IMG):
	$(DOCKER) make -C ./src
	cp src/$(KERNEL_IMG) $@