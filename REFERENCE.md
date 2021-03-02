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
