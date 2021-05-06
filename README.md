# OSDI 2020 - LAB 05 Thread and User Process

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856167    | Yunyung        | 許振揚| yungyung7654321@gmail.com  |

### Introduction
Multitasking is the most important feature of an operating system. In this lab, we’ll learn how to create threads and how to switch between different threads to achieve multitasking. Moreover, we’ll learn how a user program becomes a user process and accesses services provided by the kernel through system calls.

### Goals of this lab
- Understand how to create threads and user processes.

- Implement create theads and user processes.

- Implement syscall such as fork(), exec(), and so on.

- Understand how to implement scheduler and context switch.

- Implement round robin scheduler and context switch mechanism.

- Understand what’s preemption.

- Implement user and kernel preemption

- Understand how to implement the waiting mechanism.

- Implement the waiting machanism 

- Understand what's POSIX signals and mechanism.

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
│   └── wait.h
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
│   └── sched.c  
│ 
├── rootfs              # file will be made as cpio archive / user program
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
     
