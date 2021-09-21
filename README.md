# OSDI 2021 - Lab 08 Virtual Memory

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856167    | Yunyung        | 許振揚| yungyung7654321@gmail.com  |

### Introduction
Virtual memory provides isolated address spaces, so each user process can run in its address space without interfering with others.

In this lab, we need to initialize the memory management unit(MMU) and set up the address spaces for the kernel and user processes to achieve process isolation

### Goals of this lab
- Understand ARMv8-A virtual memory system architecture.

- Understand how to design multitasking with virtual memory.

- Understand how to prevent invalid memory access.

- Understand how the kernel manages memory for user processes.

- Implement simple mmap

- Understand how demand paging works.

- Implement demand paging mechanism.

- Understand how copy-on-write works.

## Directory structure
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
│   ├── uart.h          # header file to define address of miniUart register and functions 
│   ├── cpio.h          # header file to parse  cpio archive function       
│   ├── mm.h            # header file to provide memory allocation
│   ├── sysregs.h   
│   ├── base.h    
│   ├── utils.h
│   ├── entry.h
│   ├── exception.h
│   ├── timer.h
│   ├── fork.h
│   ├── sched.h
│   ├── sys.h
│   ├── wait.h
│   ├── vfs.h
│   ├── fs.h
│   ├── tmpfs.h
│   ├── mbr.h           # header file of Master boot Record
│   ├── sdhost.h    
│   ├── fat32.h  
│   ├── base.h  
│   └── mmu.h
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
│   ├── uart.c          # source file to process uart interface and implement uart asynchronous read/write cooperate with shell in shell.c
│   ├── mm.c            # Implementation of buudy system and object allocator(slab) for memory allocation
│   ├── mm.S   
│   ├── utils.c 
│   ├── utils.S        
│   ├── entry.S 
│   ├── exception.c  
│   ├── timer.S 
│   ├── timer.c 
│   ├── fork.c 
│   ├── sys.S 
│   ├── sys.c
│   ├── wait.c 
│   ├── sched.S
│   ├── sched.c
│   ├── vfs.c 
│   ├── fs.c
│   ├── tmpfs.c
│   ├── sdhost.c        # Implmentation of sd care device driver
│   └── fat32.c
│ 
├── sdcard              # Files for sd card device (Lab7)
│   ├── sfn_nctuos.img  # Flash Bootable Image for SD Card with FAT32 file system. FAT32 is Short Filenames(SFN) version.
│   └── ...             # Other simple files to test Lab7
│
├── rootfs              # files and user programs will be made as cpio archive
│   └── ...             # any file 
│
├── .gitignore
├── initramfs.cpio      # cpio archive contain files and user programs
├── LICENSE
├── link.ld             # linker script
├── Makefile 
├── config.txt          # config file to rpi3's GPU bootloader 
├── README.md
```


## Simple Shell
**In this lab, kernel shell is disable**
| command           | description                        | 
| ------------------| -----------------------------------| 
| hello             | print Hello World!                 |
| help              | print all available commands       |
| timestamp         | print current timestamp            |
| reboot            | reset rpi3                         |
| ls                | list cpio file                     |
| cat {filename}    | print cpio content in {filename}   |
| ma                | Test Memory allocation             |
| currentEL         | Piint current exception level(You can't use it in EL0) | 
| cpio_svc          | Jump to user program in cpio archive |
| coreTimerOn       | Enable core0 timer interrupt (Interrupt periodically) |
| coreTimerOff      | Disable core0 timer interrupt |
| setTimeout [MESSAGE] [SECONDS] | prints [MESSAGE] after [SECONDS] ([SECONDS] need lower than 45) |

## How to build
```
make
```

## How to clean the build
```
make clean
```

## Run on QEMU
```
# Run QEMU with initramfs.cpio and sd card
make run_all
# Run QEMU without initramfs.cpio
make run_cpio
# Run QEMU without initramfs.cpio and sd card
make run 

```

## How to interact with Rpi3
- miniUART
```
$ sudo screen /dev/ttyUSB0 115200
```
- miniUART with printed message saved to Logfile
```
$ sudo screen -L -Logfile log.txt /dev/ttyUSB0 115200
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
    $ sudo pip3 install pyserial
    ```
     
