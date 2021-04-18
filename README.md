# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`309551092`| `samuelyen36` | `顏劭庭` | samuelyen.cs05@g2.nctu.edu.tw |

## How to build
- Simply type ```make``` to build the OS
- It would generate two .img files, which are used to meet the requirement of this homework

## How to run
- ```make run``` to start the enumerator

## How to burn it into pi3
I follow the instruction in lab0, loading bootcode.bin, fixup.dat and start.elf into SD card, and also put our kernel8.img into Rpi3. Then, we can start the board to check the result.

## Architecture
=======
1. Replace the ```bootloader.img``` file and ```config.txt``` to the new one which previous ```make``` had created
2. You can connect to your pi3 by UART

## Note
remove > kernel_address=0x70000 from config.txt

<!-- ## Architecture

**WIP**

## Directory structure

**WIP** -->
