
#include <string.h>

bool String::Equal(const char* str1, const char* str2) {
    while (true) {
        if (*str1 != *str2) {
            return false;
        }
        else if (*str1 == 0) {
            return true;
        }
        str1++;
        str2++;
    }
}
