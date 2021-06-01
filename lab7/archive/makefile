.PHONY:on off getoffset
FILE:=sfn_nctuos.img

on:
	sudo losetup -P /dev/loop10 $(FILE)
	sudo mount -t msdos -o offset=1048576 /dev/loop10 /mnt
off:
	sudo umount /mnt
	sudo losetup -d /dev/loop9

getoffset:
	sudo fdisk -l -u=sectors $(FILE)
