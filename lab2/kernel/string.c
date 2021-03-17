#include "string.h"

int strcmp(const char *X, const char *Y){
    while(*X){
        if (*X != *Y){
            break;
        }
        X++;
        Y++;
    }
    return *(const unsigned char*)X - *(const unsigned char*)Y;
}

int strncmp(const char *X, const char *Y, size_t n){
    while(n--){
        if (*X != *Y){
            return 1;
        }
        X++;
        Y++;
    }
    return 0;
}

char *strtok(char *str, char delim){
    while(*str){
        if(*str == delim){
            *str = '\0';
            str++;
            while(*str){
                if(*str != delim){
                    return str;
                }
                str++;
            }
            return NULL;
        }
        str++;
    }
    return NULL;
}

void strrev(char *head){
    if(!head){
        return;
    }

    char *tail = head;
    while(*tail){
        tail++;
    };
    tail--;

    while(head < tail){
        char h = *head;
        *head++ = *tail;
        *tail-- = h;
    }
}

char *trim(char *head){
    while(*head ==  ' '){
        head++;
    }

    if(*head == '\0'){
        return head;
    }

    char *tail = head;
    while(*tail){
        tail++;
    };
    tail--;

    while(*tail == ' '){
        tail--;
    }

    *(tail+1) = '\0';
    return head;
}

uint32_t xatou32(const char *str, size_t len){
    uint32_t result = 0;
    while(*str && len){
        result = result << 4;
        if(*str >= '0' && *str <= '9'){
            result |= *str - '0';
        }else if(*str >= 'A' && *str <= 'F'){
            result |= *str - 'A' + 10;
        }else if(*str >= 'a' && *str <= 'f'){
            result |= *str - 'a' + 10;
        }else{
            return 0;
        }
        *str++;
        len--;
    }
    return result;
}

int uitoa(uint32_t num, char *buffer, size_t buffer_size, uint32_t base){
    if(buffer_size < 2 || base < 2 || base > 16){
        return 1;
    }

    size_t i = 0;
    if(!num){
        buffer[i++] = '0';
    }

    while(num){
        if(i == buffer_size-1){
            buffer[i] = '\0';
            return 1;
        }
        uint32_t rem = num % base;
        buffer[i++] = (rem > 9)? (rem - 10) + 'a' : rem + '0';
        num /= base;
    }

    buffer[i] = '\0';
    strrev(buffer);
    return 0;
}
