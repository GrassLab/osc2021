# NCTU Operating Capstone 2021

## Author

| Student ID | GitHub Account | Name  | Email                       |
| -----------| -------------- | ----- | --------------------------- |
| 309551054  | Brothre23      | 于兆良 | daveyu824.cs09g@nctu.edu.tw |

## How To Run

- Build Kernel

 ~~~shell
 cd Kernel
 make
 cd ..
 ~~~

- Build Bootloader & Deploy to Raspberry Pi

 ~~~shell
 cd loader
 make
 make deploy
 cd ..
 ~~~

- Send Kernel

 ~~~shell
 sudo python writer.py
 ~~~
  
 (Remember to run as sudo, or you will get an error.)

## Simple Shell

| command   | description                   |
| ----------| ----------------------------- |
| hello     | print Hello World!            |
| help      | print all available commands  |
| timestamp | print current timestamp       |
| reboot    | reset rpi3                    |
| cpio      | read initramfs.cpio           |

## Directory Structure

~~~shell
├── LICENSE
├── README.md
├── initramfs.cpio
├── kernel
│   ├── Makefile
│   ├── build
│   ├── include
│   │   ├── command.h
│   │   ├── cpio.h
│   │   ├── gpio.h
│   │   ├── math.h
│   │   ├── printf.h
│   │   ├── shell.h
│   │   ├── string.h
│   │   └── uart.h
│   ├── link.ld
│   ├── src
│   │   ├── command.c
│   │   ├── cpio.c
│   │   ├── main.c
│   │   ├── math.c
│   │   ├── printf.c
│   │   ├── shell.c
│   │   ├── string.c
│   │   └── uart.c
│   └── start.S
├── loader
│   ├── Makefile
│   ├── build
│   ├── include
│   │   ├── gpio.h
│   │   ├── math.h
│   │   ├── printf.h
│   │   └── uart.h
│   ├── link.ld
│   └── src
│       ├── main.c
│       ├── math.c
│       ├── printf.c
│       ├── start.S
│       └── uart.c
└── writer.py

~~~
