# My OSC 2021 - LAB 01

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`L091197`| `andykuo8766` | `郭紘安` | andykuo8766@gapp.nthu.edu.tw |

## Files
| File          | Content                                               | 
| --------------| ----------------------------------------------------- | 
| command.c(.h) | code for action to deal with different shell command  |
| gpio.c        | some gpio config                                      |
| main.c        | main program                                          |
| math.c(.h)    | code for replace standard math.h                      |
| shell.c(.h)   | code for control the shell behave                     |
| string.c(.h)  | code for replace standard math.h                      |
| uart.c(.h)    | code for uart opertaion                               |
| link.ld       | linker script                                         |

## How to build
```bash
make
```
## Run on QEMU
```bash
make run
```
## Simple Shell
| command   | description                   | 
| ----------| ----------------------------- | 
| hello     | print Hello World!            |
| help      | print all available commands  |
| reboot    | reboot rpi3                   |
| cancel    | cancel reboot rpi3            |
==================
Lab 1: Hello World
==================

************
Introduction
************

In Lab 1, you will practice bare metal programming by implementing a simple shell.
You need to set up mini UART, and let your host and rpi3 can communicate through it.

*****
Goals
*****

* Practice bare metal programming.
* Understand how to access rpi3’s peripherals.
* Set up mini UART.

********
Required
********

Requirement 1
=============

Basic Initialization
--------------------

When a program is loaded, it requires,

* All it's data is presented at correct memory address.
* The program counter is set to correct memory address.
* The bss segment are initialized to 0.
* The stack pointer is set to a proper address.

After rpi3 booting, its booloader loads kernel8.img to physical address 0x80000,
and start executing the loaded program.
If the linker script is correct, the above two requirements are met.

However, both bss segment and stack pointer are not properly initialized.
Hence, you need to initialize them at very beginning.
Otherwise, it may lead to undefined behaviors.

``required 1`` Initialize rpi3 after booted by bootloader.

Requirement 2
=============

Mini UART 
---------

You'll use UART as a bridge between rpi3 and host computer for all the labs.
Rpi3 has 2 different UARTs, mini UART and PL011 UART.
In this lab, you need to set up the mini UART.

``required 2`` Following :ref:`uart` to set up mini UART.

Requirement 3
=============

Simple Shell
------------

After setting up UART, you should implement a simple shell to let rpi3 interact with the host computer.
The shell should be able to execute the following commands.

======== ============================
command  Description
======== ============================
help     print all available commands
hello    print Hello World!
======== ============================

``required 3`` Implement a simple shell supporting the listed commands.

********
Elective
********

Reboot
======

Rpi3 doesn't provide an on board reset button.

You can follow example code to reset your rpi3.

.. code-block:: c

  #define PM_PASSWORD 0x5a000000
  #define PM_RSTC 0x3F10001c
  #define PM_WDOG 0x3F100024

  void reset(int tick){ // reboot after watchdog timer expire
    set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
    set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick
  }

  void cancel_reset() {
    set(PM_RSTC, PM_PASSWORD | 0); // full reset
    set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
  }


``elective 1`` Add <reboot> command.

.. note::
  This snippet of code only works on real rpi3, not on QEMU.
