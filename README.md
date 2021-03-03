# NCTU Operating Capstone 2021

## Author

| Student ID | GitHub Account | Name  | Email                       |
| -----------| -------------- | ----- | --------------------------- |
| 309551054  | Brothre23      | 于兆良 | daveyu824.cs09g@nctu.edu.tw |

## Files

| File          | Content                               |
| --------------| --------------------------------------|
| command.c(.h) | dealing with different shell commands |
| gpio.c        | some gpio config                      |
| main.c        | main program                          |
| math.c(.h)    | replica of the standard math.h        |
| shell.c(.h)   | controlling the shell behavior        |
| string.c(.h)  | replica of the standard string.h      |
| start.S       | setting up the booting process        |
| uart.c(.h)    | uart opertaion                        |
| link.ld       | linker script                         |

## Build

```bash
make
```

## Run on QEMU

```bash
make run
```

## Deploy to Pi 3

```bash
make deploy
```

(Remember to change the mounting point.)

## Simple Shell

| command   | description                   |
| ----------| ----------------------------- |
| hello     | print Hello World!            |
| help      | print all available commands  |
| timestamp | print current timestamp       |
| reboot    | reset rpi3                    |
