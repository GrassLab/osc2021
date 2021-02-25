# Operating System Capstone 2021

My Implementation of Operating System at [`OSC2021`][OSC21], NYCU.

[OSC21]: https://grasslab.github.io/NYCU_Operating_System_Capstone/

## Author

|   學號    | GitHub 帳號 | 姓名     | Email                    |
| :-------: | ----------- | -------- | :----------------------- |
| `0856039` | `ianre657`  | `陳奕安` | ianchen.cs08@nycu.edu.tw |

## Prerequisite

+ Build only
  + [docker][docker]
+ Dev tool
  + [clang-format][clang-format]: Format C code
  + [qemu][qemu]: Run kernel image in emulater
  + [Raspberry Pi Imager][imger]: SD card utilites

[qemu]: https://www.qemu.org/download/
[docker]: https://www.docker.com/
[clang-format]: https://formulae.brew.sh/formula/clang-format
[imger]: https://www.raspberrypi.org/software/

## How-to

| Usage            | Command      | Description                                    |
| :--------------- | :----------- | :--------------------------------------------- |
| Build code       | `make`       | Generate kernel image:`kernel8.img`            |
| Run              | `make run`   | Run kernel image in `qemu`                     |
| Enter Virtualenv | `make shell` | Start a bash shell (with cross-compiling tool) |

## How to burn it into pi3

**WIP**

## Architecture

**WIP**

## Directory structure

**WIP**
