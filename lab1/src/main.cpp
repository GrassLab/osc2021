#include <kernel.h>
#include <terminal.h>

extern "C" void __my_main__() {
    Kernel::InitBss();
    Terminal().Run();
}
