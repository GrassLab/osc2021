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
  + A corss-gdb for arrch64: see below

[qemu]: https://www.qemu.org/download/
[docker]: https://www.docker.com/
[clang-format]: https://formulae.brew.sh/formula/clang-format
[imger]: https://www.raspberrypi.org/software/

### Compiling a cross gdb for `aarch64`

1. Download `gdb` from [official gdb website][gdb]
2. unzip code and `cd` into folder
3. `./configure --target=aarch64-linux`
4. `make`
5. `make install`, (the target binary would be installed as `/usr/local/bin/aarch64-linux-gdb`)

[gdb]: https://www.gnu.org/software/gdb/download/

## How-to

| Usage            | Command      | Description                                    |
| :--------------- | :----------- | :--------------------------------------------- |
| Build code       | `make`       | Generate kernel image:`kernel8.img`            |
| Setup            | `make setup` | Rebuild docker image for cross-compiling       |
| Run              | `make run`   | Run kernel image in `qemu`                     |
| Enter Virtualenv | `make shell` | Start a bash shell (with cross-compiling tool) |

## How to burn it into pi3

**WIP**

## Architecture

**WIP**

## Directory structure

**WIP**
