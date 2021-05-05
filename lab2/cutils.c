char *align_upper(char *addr, int alignment)
{
    char *res;
    int r = (unsigned long)addr % alignment;
    res = r ? addr + alignment - r : addr;
    return res;
}

char *align_down(char *addr, int alignment)
{
    int r = (unsigned long)addr % alignment;
    return addr - r;
}

int strlen(char *str)
{ // '\0' doesn't count.
    int cnt;
    char *str_ptr;

    cnt = 0;
    str_ptr = str;
    while (*str_ptr++ != '\0')
        cnt++;
    return cnt;
}

void *memcpy(char *str1, const char *str2, int n)
{
    for (int i = 0; i < n; ++i)
        str1[i] = str2[i];
    return 0;
}

int strcpy(char *str1, const char *str2)
{ // return str2 length without counting '\0'
    int i = 0;
    while (str2[i] != '\0') {
        str1[i] = str2[i];
        i++;
    }
    str1[i] = '\0';

    return i;
}

int strcmp(const char *str1, const char *str2)
{
    int i;

    i = 0;
    while (str1[i] != '\0') {
        if (str1[i] != str2[i])
            return 1;
        i++;
    }
    if (str2[i] != '\0')
        return 1;
    return 0;
}

int strcmp_with_len(char *str1, char *str2, int len)
{ // compare two strings with at most len characters
    for (int i = 0; i < len; ++i) {
        if (str1[i] != str2[i])
            return 1;
        // if come to here, str1[i] == str2[i]
        if (str1[i] == '\0')
            return 0;
    }
    return 0;
}

char *strstr(char *haystack, char *needle)
{ // return pointer of needle firstly appears in haystack.
    char *ptr1;
    int search_len, pat_len;

    pat_len = strlen(needle);
    search_len = strlen(haystack) - pat_len;
    for (int i = 0; i <= search_len; ++i) {
        ptr1 = haystack + i;
        if (!strcmp_with_len(ptr1, needle, pat_len))
            return ptr1;
    }
    return 0; // Find no needle, return NULL.
}

int hex_string_to_int(char *hex_str, int len)
{
    int num, base;
    char *ch;

    num = 0;
    base = 1;
    ch = hex_str + len - 1;
    while(ch >= hex_str) {
        if (*ch >= 'A')
            num += (base*(10 + *ch - 'A'));
        else
            num += (base*(*ch - '0'));
        base *= 16;
        ch--;
    }
    return num;
}

unsigned long hex_string_to_unsigned_long(char *hex_str, int len)
{
    unsigned long num;
    int base;
    char *ch;

    num = 0;
    base = 1;
    ch = hex_str + len - 1;
    while(ch >= hex_str) {
        if (*ch >= 'A')
            num += (base*(10 + *ch - 'A'));
        else
            num += (base*(*ch - '0'));
        base *= 16;
        ch--;
    }
    return num;
}

int dec_string_to_int(char *dec_str, int len)
{
    unsigned long num;
    int base;
    char *ch;

    num = 0;
    base = 1;
    ch = dec_str + len - 1;
    while(ch >= dec_str) {
        num += (base*(*ch - '0'));
        base *= 10;
        ch--;
    }
    return num;
}

int bit_test(char *buf, int bit)
{
    return buf[bit / 8] & (1 << (bit % 8));
}


int bit_set(char *buf, int bit)
{
    buf[bit / 8] |= 1 << (bit % 8);
    return 0;
}

int bit_clr(char *buf, int bit)
{
    buf[bit / 8] &= ~(1 << (bit % 8));
    return 0;
}
