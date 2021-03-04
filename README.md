# My OSDI 2020 - LAB 01

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856167    | Yunyung        | 許振揚| yungyung7654321@gmail.com  |

### Introduction
In Lab 1, we will practice bare metal programming by implementing a simple shell. we need to set up mini UART, and let our host and rpi3 can communicate through it.

## Directory structure

```
.
├── include             # header files
│   ├── command.h       # header file to process command
│   ├── gpio.h          # header file to define some constant address
│   ├── math.h          # header file to implement some function in <math.h>
│   ├── shell.h         # header file to process shell flow
│   ├── string.h        # header file to implement some function in <string.h>
│   └── uart.h          # header file to process uart interface
│
├── src                 # source files
│   ├── command.c       # source file to process command
│   ├── main.c          # int main
│   ├── shell.c         # source file to process shell flow
│   ├── start.S         # source code for booting
│   ├── string.c        # source file to implement some function in <string.h>
│   ├── math.c        # source file to implement some function in <math.h>
│   └── uart.c          # source file to process uart interface
│
├── LICENSE
├── link.ld             # linker script
├── Makefile
├── README.md
```

## Files
| File          | Content                                               | 
| --------------| ----------------------------------------------------- | 
| start.S       | Assembly code to configure some setting when booting  |
| main.c(.h)    | main program                                          |
| shell.c(.h)   | code for control the shell behave                     |
| command.c(.h) | code for action to deal with different shell command  |
| uart.c(.h)    | code for uart opertaion                               |
| gpio.h        | some gpio config                                      |
| math.c(.h)    | code for replace standard math.h                      |
| string.c(.h)  | code for replace standard math.h                      |
| link.ld       | linker script                                         |

## Simple Shell
| command   | description                   | 
| ----------| ----------------------------- | 
| hello     | print Hello World!            |
| help      | print all available commands  |
| timestamp | print current timestamp       |
| reboot    | reset rpi3                    |



## How to build

```
make
```

## Run on QEMU
```
make run_qeum
```