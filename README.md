# NCTU Operating Capstone 2021

## Author

| Student ID | GitHub Account | Name  | Email                       |
| -----------| -------------- | ----- | --------------------------- |
| 309551054  | Brothre23      | 于兆良 | daveyu824.cs09g@nctu.edu.tw |

## How To Run

- Build

 ~~~shell
 make
 ~~~

- Run on QEMU

 ~~~shell
 make run
 ~~~

## Usage

~~~c
void *address = memory_allocation(size);
memory_free(address);
~~~

## Directory Structure

~~~shell
├── LICENSE
├── Makefile
├── README.md
├── build
├── include
│   ├── command.h
│   ├── cpio.h
│   ├── gpio.h
│   ├── list.h
│   ├── math.h
│   ├── memory.h
│   ├── printf.h
│   ├── shell.h
│   ├── string.h
│   └── uart.h
├── link.ld
├── src
│   ├── command.c
│   ├── cpio.c
│   ├── main.c
│   ├── math.c
│   ├── memory.c
│   ├── printf.c
│   ├── shell.c
│   ├── string.c
│   └── uart.c
└── start.S
~~~
