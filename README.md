# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856085`| `t0037799` | `林濬維` | jwlin.cs08g@nctu.edu.tw |

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
| myshell.c(.h) | code for control the shell behavior                   |
| uart.c(.h)    | code for uart opertaion                               |
| linker.ld     | linker script                                         |
| start.S       | startup program                                       |
