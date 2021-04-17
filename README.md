# OSDI 2020 - LAB 04  Exception and Interrupt

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856167    | Yunyung        | 許振揚| yungyung7654321@gmail.com  |

### Introduction
An exception is an event that causes the currently executing program to relinquish the CPU to the corresponding handler. With the exception mechanism, an operating system can

1. do proper handling when an error occurs during execution.

2. A user program can generate an exception to get the corresponding operating system’s service.

3. A peripheral device can force the currently executing program to relinquish the CPU and execute its handler.

### Goals of this lab
- Understand what’s exception levels in Armv8-A.

- Understand what’s exception handling.

- Understand what’s interrupt.

- Understand how rpi3’s peripherals interrupt the CPU by interrupt controllers.

- Implement synchronous exception and asynchronous interrupt handler from peripheral devices in Armv8-A.

- Load an user program and switch between kernel mode(EL1) and user mode(EL0).
 
- Implement core timer interrput handler and miniUART asynchronous read/write.

- Understand how to multiplex a timer.

- Implenment a timer multiplexing.

- Understand how to concurrently handle I/O devices.

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
│   ├── uart.h          # header file to define address of miniUart register and functions 
│   ├── cpio.h          # header file to parse  cpio archive function       
│   ├── mm.h            # header file to provide memory allocation
│   ├── sysregs.h   
│   ├── base.h    
│   ├── utils.h
│   ├── entry.h
│   ├── exception.h
│   └── timer.h
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
│   ├── utils.S        
│   ├── entry.S 
│   ├── exception.c  
│   ├── timer.S   
│   └── timer.c    
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
| currentEL         | Piint current exception level(You can't use it in EL0) | 
| cpio_svc          | Jump to user program in cpio archive |
| coreTimerOn       | Enable core0 timer interrupt (Interrupt periodically) |
| coreTimerOff      | Disable core0 timer interrupt |
| setTimeout [MESSAGE] [SECONDS] | prints [MESSAGE] after [SECONDS] ([SECONDS] need lower than 45) |

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
make run_cpio
```

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
     
