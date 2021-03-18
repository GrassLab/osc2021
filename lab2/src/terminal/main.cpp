#include <kernel.h>
#include <terminal.h>

void main(void(*dtd_addr)) {
    Terminal().Run();
}
