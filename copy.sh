sudo mount /dev/sdb1 ~/mountDisk && \
sudo cp ./build/bootloader.img ~/mountDisk && \
sudo cp ./build/config.txt ~/mountDisk && \
sudo cp ./build/initramfs.cpio ~/mountDisk && \
sudo umount ~/mountDisk 
