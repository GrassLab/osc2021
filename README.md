# My OSC 2021 - LAB 02

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
```bootloader
```bash
cd bootloader
make
```


## Simple Shell
| command   | description                   | 
| ----------| ----------------------------- | 
| hello     | print Hello World!            |
| help      | print all available commands  |
| reboot    | reboot rpi3                   |
| cancel    | cancel reboot rpi3            |
