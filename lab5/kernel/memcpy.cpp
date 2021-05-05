#include "types.h"
extern"C"
void memcpy(char* dst, char* src, uint64_t count) {
    while (count > 0) {
        *(dst++) = *(src++);
        count--;
    }
}
