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

### Fixed Timer

- in `./src/asm/exception.S`

~~~asm
    // Current EL with SPx
    .align 7
    b svc_handler           // Synchronous
    .align 7
    b fixed_timer_handler   // IRQ
    .align 7
    b invalid_handler       // FIQ
    .align 7
    b invalid_handler       // SError
 ~~~

- to enable timer: `timer_on`
- to disable timer: `timer_off`

### User Timer

- in `./src/asm/exception.S`

~~~asm
     // Current EL with SPx
    .align 7
    b svc_handler           // Synchronous
    .align 7
    b user_timer_handler    // IRQ
    .align 7
    b invalid_handler       // FIQ
    .align 7
    b invalid_handler       // SError
~~~

- to add an new timer:

~~~shell
set_timeout
time: 10
message: HAHA10
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
<<<<<<< HEAD
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
=======
│   ├── mm.h
│   ├── printf.h
│   ├── shell.h
│   ├── string.h
│   ├── sys_regs.h
│   ├── timer.h
│   └── uart.h
├── initramfs.cpio
├── link.ld
└── src
    ├── asm
    │   ├── exception.S
    │   ├── start.S
    │   └── timer.S
    └── c
        ├── command.c
        ├── cpio.c
        ├── exception.c
        ├── main.c
        ├── math.c
        ├── mm.c
        ├── printf.c
        ├── shell.c
        ├── string.c
        ├── timer.c
        └── uart.c
>>>>>>> de7a622e18bcb8ba0576355526bc99984106c5aa
~~~
