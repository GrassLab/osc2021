# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0616325`| `cclin0816` | `林重均` | benjamin.cclin@gmail.com |


## How to use

```
make [run / debug / burn / genfs / clean]
```

* run - run w/ qemu
* debug - debug w/ qemu
* burn - burn to sd card (default /dev/sdb1 change makefile to modify)
* genfs - manually regenerate initramfs.cpio with rootfs folder
* clean - clear repo

## Directory structure
```
osc2021/
├─ LICENSE ........ license
├─ README.md ...... this thing
├─ bl_linker.ld ... linker script for bootloader.elf
├─ config.txt ..... config for raspi
├─ kn_linker.ld ... linker script for kernel8.elf
├─ makefile ....... make script
├─ script.py ...... script to interactive w/ shell on raspi
├─ include/ ....... include header files
├─ rootfs/ ........ rootfs for initramfs.cpio
└─ src/ ........... source files
  ```