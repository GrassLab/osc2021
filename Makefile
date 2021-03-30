# OSC 2021, check https://github.com/GrassLab/osc2021
include Makefile.inc

# set build parameters
SD_MEDIA    ?= /run/media/calee/4DFF-0A36
BOOTLOADER  = bootloader
OS          = os
BOOT_IMG    = bootloader.img
OS_IMG      = kernel8.img

.PHONY: all clean install

all:
	@echo "Start building bootloader image..."
	make -C $(BOOTLOADER)
	cp $(BOOTLOADER)/$(BOOT_IMG) ./$(BOOT_IMG)
	@echo "$(BOOTLOADER) image build completed!"
	@echo "==============================="
	@echo "Start building OS image..."
	make -C $(OS)
	cp $(OS)/$(OS_IMG) ./$(OS_IMG)
	@echo "$(OS) image build completed!"
	@echo "==============================="
	@echo "Build complate!"

clean:
	make -C $(BOOTLOADER) clean
	make -C $(OS) clean

install:
#cp ./setup/* $(SD_MEDIA)
	cp $(BOOTLOADER)/config.txt $(SD_MEDIA)
	cp $(BOOT_IMG) $(SD_MEDIA)

uninstall:
	rm $(SD_MEDIA)/config.txt
	cp ./setup/kernel8.img $(SD_MEDIA)
