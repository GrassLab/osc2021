sudo mount /dev/sdb1 ~/mountDisk && \
sudo cp ./build/bootloader.img ~/mountDisk && \
sudo cp ./assets/config.txt ~/mountDisk && \
sudo cp ./build/initramfs.cpio ~/mountDisk && \
sudo umount ~/mountDisk 
