.PHONY:all clean

all:
	make clean
	cd rootfs&&find . | cpio -o -H newc > ../initramfs.cpio


clean:
	-rm *.cpio
