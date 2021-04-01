# My OSC 2021
## Author
| 學號        | GitHub 帳號  | 姓名    | Email                      |
| ---------- | ----------- | ------- | -------------------------- |
| `A091513`  | `naihsin` | `張乃心` | s109164507@m109.nthu.edu.tw  |

## How to build loader
```bash
cd loader
make
```

## How to build kernel
```bash
cd kernel
make
```

## How to build cpio archive file
```bash
cd rootfs
find . | cpio -o -H newc > ../initramfs.cpio
```

## How to run loader
```bash
qemu-system-aarch64 -M raspi3 -kernel bootloader.img -initrd initramfs.cpio -serial null -serial pty
screen /dev/<dev_name>
```

## How to run kernel
```bash
qemu-system-aarch64 -M raspi3 -kernel kernel8.img -initrd initramfs.cpio -serial null -serial pty
screen /dev/<dev_name>
```

## How to burn and run it into pi3
- Copy bootloader.img to your SD card
- Copy initramfs.cpio to your SD card
- Restart raspi3
- Waiting bootloader shell
- Type loadimg command
- Open another terminal to send kernel.img
```bash
python3 sender.py 
```
- Tab to raspi3 console, check for the recving kernel.img
- Type jumpimg commmand
- You will see the new kernel shell
- Type cpio command

## Directory structure

| File / Directory | Content                             | 
| --------------| ----------------------------------------------------- | 
| include       | C header file                                      |
| kernel        | Kernel main program, lib, makefile etc ...            |
| loader        | Loader main program, lib, makefile etc ...            |
| initramfs.cpio    | Cpio Archive file                               |
| sender.py      | Python script for sending kernel.img               |
