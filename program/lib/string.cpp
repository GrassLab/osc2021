#include <string.h>

size_t strlen(const char * str) {
    int offset = 0;
    while (str[offset] != 0) {
        offset++;
    }
    return offset;
}

char* u64toa(uint64_t number, char *output, size_t count) {
    char buffer[25];
    int offset = 0;
    if (count < 2) {
        return nullptr;
    }
    while (number) {
        buffer[offset] = '0' + (number % 10);
        number = number / 10;
        offset++;
        if (count < offset + 1) {
            return nullptr;
        }
    }
    if (offset == 0) {
        strcpy(output, "0");
        return output;
    }
    for (int i = offset; i > 0; i--) {
        output[offset - i] = buffer[i - 1];
    }
    output[offset] = 0;
    return output;
}

char* u64tohex(uint64_t number, char *output, size_t count) {
    if (count < 19) {
        return nullptr;
    }
    output[0] = '0';
    output[1] = 'x';
    for (int i = 15; i >= 0; i--) {
        char ch = (number >> (i << 2)) & 0xf;
        if (ch <= 9) ch += '0';
        else ch += ('a' - 10);
        output[15 - i + 2] = ch;
    }
    output[18] = 0;
    return output;
}

char* strcpy(char* dst, const char* src) {
    uint64_t count = 0;
    while (src[count]) {
        dst[count] = src[count];
        count++;
    }
    dst[count] = src[count];
    count++;
    return dst;
}

int strcmp(const char *p1, const char *p2)
{
    const unsigned char *s1 = (const unsigned char *) p1;
    const unsigned char *s2 = (const unsigned char *) p2;
    unsigned char c1, c2;
    do {
          c1 = (unsigned char) *s1++;
          c2 = (unsigned char) *s2++;
          if (c1 == '\0' || c2 == '\0')
              return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

char *
strtok_r(char *s, const char *delim, char **last)
{
    char *spanp;
    int c, sc;
    char *tok;

    if (s == nullptr && (s = *last) == nullptr)
    {
	return nullptr;
    }

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0; )
    {
	if (c == sc)
	{
	    goto cont;
	}
    }

    if (c == 0)		/* no non-delimiter characters */
    {
	*last = nullptr;
	return nullptr;
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;)
    {
	c = *s++;
	spanp = (char *)delim;
	do
	{
	    if ((sc = *spanp++) == c)
	    {
		if (c == 0)
		{
		    s = nullptr;
		}
		else
		{
		    char *w = s - 1;
		    *w = '\0';
		}
		*last = s;
		return tok;
	    }
	}
	while (sc != 0);
    }
    /* NOTREACHED */
}

char * strtok(char *s, const char *delim)
{
    static char *last;

    return strtok_r(s, delim, &last);
}

extern"C"
void *memset(void *s, int c, size_t n) {
    for (int i = 0; i < n; i++) {
        ((char*)s)[i] = (char)c;
    }
    return s;
}

extern"C"
void *memcpy(void *dest, const void *src, size_t n) {
    for (int i = 0; i < n; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }
    return dest;
}
