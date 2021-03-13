# Operating System Capstone 2021

My Implementation of Operating System at [`OSC2021`][OSC21], NYCU.

|   學號    | GitHub 帳號 | 姓名     | Email                       |
| :-------: | ----------- | -------- | :-------------------------- |
| `0856039` | `ianre657`  | `陳奕安` | ianchen.cs08[at]nycu.edu.tw |

![demo][demo]

[OSC21]: https://grasslab.github.io/NYCU_Operating_System_Capstone/
[demo]: ./demo/lab2.gif

## Prerequisite

+ Required
  + [qemu][qemu]
  + [Python3.7+][python]: https://www.python.org/downloads/
  + Dependency for build scripts: `pip3 install -r requirements.txt`
+ Build for C implementation
  + [docker][docker]
  + [clang-format][clang-format]: Format C code
+ Build for Rust implementation
  1. Install [rust compiler][rs-install]
  2. Change your toolchain to `nightly`
  3. Install [cargo-binutils][cargo-binutils]

[python]: https://www.python.org/downloads/
[rs-install]: https://www.rust-lang.org/tools/install
[qemu]: https://www.qemu.org/download/
[docker]: https://www.docker.com/
[clang-format]: https://formulae.brew.sh/formula/clang-format
[cargo-binutils]: https://github.com/rust-embedded/cargo-binutils

## How-to

| Usage                | Command          | Description                                    |
| :------------------- | :--------------- | :--------------------------------------------- |
| Build code           | `make`           | Generate output to `res`                       |
| Cleanup binary files | `make clean`     | Delete all outputs                             |
| Run Kernel           | `make run-stdio` | Generate kernel image:`kernel8.img`            |
| Enter Virtualenv     | `make shell`     | Start a bash shell (with cross-compiling tool) |


### Compiling a cross gdb for `aarch64`

1. Download `gdb` from [official gdb website][gdb]
2. unzip code and `cd` into folder
3. `./configure --target=aarch64-linux`
4. `make`
5. `make install`, (the target binary would be installed as `/usr/local/bin/aarch64-linux-gdb`)

[gdb]: https://www.gnu.org/software/gdb/download/

