qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -dtb config/bcm2710-rpi-3-b-plus.dtb -drive if=sd,file=sdcard.img,format=raw -display none -serial null -serial stdio -s
