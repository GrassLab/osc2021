# My OSC 2021

## Author

| 學號        | GitHub 帳號  | 姓名    | Email                      |
| ---------- | ----------- | ------- | -------------------------- |
| `A091514`  | `kishow01`  | `林昌德` | kishow01@gmail.com         |

## How to build
``make``

## How to run
``make run``

## How to burn it into pi3
download firmware from https://github.com/raspberrypi/firmware/tree/master/boot.

+ necessary
    + bootcode.bin
        + bootloader, loaded by the SoC on boot. 
        + setup and loads one of the start*.elf files.
    + fixup.dat
        + linker files and are matched pairs with the start*.elf files
    + start.elf
        + binary blobs (firmware) that are loaded on to the VideoCore in the SoC, which then take over the boot process. start.elf is the basic firmware.

Finally, put the firmware and your kernel image into the FAT partition.

## Architecture

**WIP**

## Directory structure

**WIP**