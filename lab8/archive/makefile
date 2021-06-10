.PHONY:on off getoffset
FILE:=sfn_nctuos.img

on:
	sudo losetup -P /dev/loop12 $(FILE)
	sudo mount -t msdos -o offset=1048576 /dev/loop12 /mnt
off:
	sudo umount /mnt
	sudo losetup -d /dev/loop12

getoffset:
	sudo fdisk -l -u=sectors $(FILE)
