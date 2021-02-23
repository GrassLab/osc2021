QEMU := qemu-system-aarch64
KERNEL_IMG := ./src/kernel8.img

DOCKER := ./dockcross-linux-arm64


.PHONY: build
build:
	$(DOCKER) make -C ./src

.PHONY: clean
clean:
	$(DOCKER) make -C ./src clean

.PHONY: shell
shell:
	$(DOCKER) bash

$(KERNEL_IMG): build

.PHONY: run
run: $(KERNEL_IMG)
	$(QEMU) -M raspi3 -kernel $(KERNEL_IMG) -display none -d in_asm
