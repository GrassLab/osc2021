# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856572`| `EricPwg` | `彭宣儒` | dz92286@gmail.com |

## How to build

`make all` 或是 `make`。
結束後會生成
* `kernel8.img` : rpi3 開機用的 image
* `initramfs.cpio` : 以 `rootfs/` 下的檔案為基礎生成的
* `bootloader/kernel8.img` : 此為 bootloader 版本的開機 image，搭配 `bootloader/uploadimg.py` 使用
* `app/img*.img` : 這些檔案是 user program，是由 `app/src/*.c` 以及 `app/src/*.S` 編譯成的

## How to run

* 一般的測試執行方法 : `make run`
* 執行時顯示指令 : `make asm`
* 執行時顯示指令，並且將 uart 輸出顯示在電腦的 `/dev/pts/?` 上 : `make pty`

## How to burn it into pi3

* 將 `kernel8.img`, `initramfs.cpio`, `config.txt` 複製到 rpi3 上。
* 或是直接將 `sd.img` 使用指令 `dd` 直接燒入。 Ex : `sudo dd if=sd.img of=/dev/sdb`

## Architecture

**WIP**

## Directory structure

**WIP**
