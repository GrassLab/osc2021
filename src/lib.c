#include "lib.h"

void sleep (unsigned long time) {
    nanosleep(time * 1000000000);
}
