#include <kernel.h>
#include <terminal.h>

void main() {
    Kernel::Init();
    Terminal().Run();
}
