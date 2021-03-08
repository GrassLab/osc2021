# My OSC 2021

## Author

| 學號        | GitHub 帳號  | 姓名    | Email                      |
| ---------- | ----------- | ------- | -------------------------- |
| `A091513`  | `naihsin` | `張乃心` | s109164507@m109.nthu.edu.tw  |

## How to build
```bash
make
```

## How to run
```bash
qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio
```

## Directory structure

| File          | Content                                               | 
| --------------| ----------------------------------------------------- | 
| gpio.h        | some gpio config                                      |
| main.c        | main program                                          |
| myshell.c(.h) | code for control the shell behave                     |
| uart.c(.h)    | code for uart opertaion                               |
| link.ld       | linker script                                         |
| start.S       | startup program                                       |
