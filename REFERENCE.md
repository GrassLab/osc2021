# Reference

## Lab 0

### linker script
- https://blog.louie.lu/2016/11/06/10分鐘讀懂-linker-scripts/
- http://wen00072.github.io/blog/2014/03/14/study-on-the-linker-script/
- [GNU LD not support on macOS](https://apple.stackexchange.com/questions/341105/ld-linker-is-missing-when-installing-gcc-using-homebrew-on-mac-os)

### Cross compile

- Using LLVM
  - [Introduction of LLVM]( https://medium.com/@zetavg/%E7%B7%A8%E8%AD%AF%E5%99%A8-llvm-%E6%B7%BA%E6%B7%BA%E7%8E%A9-42a58c7a7309)
  - https://clang.llvm.org/docs/CrossCompilation.html
  - When checking llvm cross compiling, it seems that although using clang as compiler, it still using gnu linker as the cross-linker. Since that, I give up trying lld as linker but back to GNU linker

### ASM

- wfe: http://www.wowotech.net/armv8a_arch/wfe_wfi.html

## Lab 1

### ASM

- ARM Register (32 bit): http://sp1.wikidot.com/arm
- Register 64 bit: https://insidelinuxdev.net/article/a03td4.html
- [instruction set]( https://static.docs.arm.com/ddi0596/a/DDI_0596_ARM_a64_instruction_set_architecture.pdf)
- 

### UART
- GPIO: https://www.slideshare.net/raspberrypi-tw/raspberry-pigpiolookinside

## Lab 2

- [Boot options in config.txt](https://www.raspberrypi.org/documentation/configuration/config-txt/boot.md)
- [initramfs in pi](https://raspberrypi.stackexchange.com/questions/85958/easy-backups-and-snapshots-of-a-running-system-with-lvm/85959#85959)
