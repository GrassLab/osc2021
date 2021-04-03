# My OSDI 2020 - LAB 03

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856167    | Yunyung        | 許振揚| yungyung7654321@gmail.com  |

### Goals of this lab
- Understand memory management techiques in Linux . 

- Implement a page frame allocator (Buddy Memory Allocator).

- Implement a dynamic memory allocator (Slab).

- Understand and implement a startup allocator (elective goal).
- Note: In this lab, we implement simplified version of linux's memory management strategies.

## Directory structure
mm.h and mm.c is key files in our implementation.
```
.
├── bootloader          # Bootloader to load actual kernel in run time through miniUART(UART1)
│   └── ...             
│
├── include             # header files
│   ├── command.h       # header file to process command
│   ├── gpio.h          # header file to define some constant address
│   ├── math.h          # header file to implement some function in <math.h>
│   ├── shell.h         # header file to process shell flow
│   ├── string.h        # header file to implement some function in <string.h>
│   ├── printf.h        # header file to provide a simple and small printf functionality
│   ├── list.h          # The implementation of subset of list.h in linux
│   ├── types.h         # Definition of some data type
│   ├── cpio.h          # header file to parse  cpio archive function
│   └── mm.h            # header file to provide memory allocation
│
├── src                 # source files
│   ├── command.c       # source file to process command
│   ├── main.c          # main
│   ├── shell.c         # source file to process shell flow
│   ├── start.S         # source code for booting
│   ├── string.c        # source file to implement some function in <string.h>
│   ├── math.c          # source file to implement some function in <math.h>
│   ├── printf.c        # source file to provide a simple and small printf functionality
│   ├── cpio.c          # source file to parse "New ASCII Format" cpio archive
│   ├── uart.c          # source file to process uart interface
│   └── mm.c            # Implementation of buudy system and object allocator(slab) for memory allocation
│ 
├── rootfs              # file will be made as cpio archive
│   └── ...             # any file 
│
├── initramfs.cpio      # cpio archive of rootfs folder 
├── LICENSE
├── link.ld             # linker script
├── Makefile 
├── config.txt          # config file to rpi3's GPU bootloader 
├── README.md
```


## Simple Shell
| command           | description                        | 
| ------------------| -----------------------------------| 
| hello             | print Hello World!                 |
| help              | print all available commands       |
| timestamp         | print current timestamp            |
| reboot            | reset rpi3                         |
| ls                | list cpio file                     |
| cat {filename}    | print cpio content in {filename}   |
| ma                | Test Memory allocation             |

## How to interact with Rpi3
- miniUART
```
$ sudo screen /dev/ttyUSB0 115200
```

## How to transmit actual kernel through UART
- test by python3 srcipt
    ```
    // sudo is required
    $ sudo python3 kernel2Uart.py
    ```
- The python3 code is depend on pyserial module which can install py 
    ```
    // sudo is required
    sudo pip3 install pyserial
    ```
        
## How to build
```
make
```

## Run on QEMU
```
make run
```

## Run on QEMU with cpio
```
# INITRAMFS_ADDR should set to 0x8000000. Becauase QEMU loads the cpio archive file to 0x8000000 by default.
make run_cpio
```
