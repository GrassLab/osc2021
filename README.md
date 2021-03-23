# My OSC 2021

## Author

| 學號        | GitHub 帳號  | 姓名    | Email                      |
| ---------- | ----------- | ------- | -------------------------- |
| `A091514`  | `kishow01`  | `林昌德` | kishow01@gmail.com         |

## How to build
### bootloader
```
cd bootloader
make
```

### kernel_sender
```
cd kernel_sender
make
```

### cpio kernel
```
cd cpio_kernel
make
```

## How to run
### bootloader
```
cd bootloader
make run
```

### cpio kernel
```
cd cpio_kernel
make run
```

## How to burn it into pi3
Move bootloader.img, initramfs.cpio and config.txt into sd card

## Architecture

+ bootloader
    + the uart bootloader in sd card
+ kernel_sender
    + send kernel through uart from host to rpi3
+ kernel
    + kernel that support cpio

## Directory structure

**WIP**
