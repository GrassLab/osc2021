#include "uart.h"

// calculate string length
int strlen(char *s)
{
	int size = 0;

	while(*s) 
	{
		size++;
		s++;
	}

	return size;
}

// compare two string length is equal ?
int strcmp(char *s1, char *s2)
{
	int len1 = strlen(s1);
	int len2 = strlen(s2);

	if(len1 != len2) 
		return 0;
	
	for(int i = 0; i <= len1; i++)
	{
		if(*s1 != *s2) 
			return 0;
		s1++;
		s2++;
	}

	return 1;
}
// compare two string is equal ?
int strcmpn(char *s1, char *s2, int n)
{
	for(int i = 0; i <= n; i++)
	{
		if(*s1 != *s2) 
			return 0;
		s1++;
		s2++;
	}

	return 1;
}

// string to int
int atoi(char* input)
{
    int res = 0;
    int i = 0;
    int isNegative = 0;

    while (input[i] != '\0') 
	{
        int current;

        if (i == 0 && input[i] == '-') 
		{
            isNegative = 1;
            i++;
            continue;
        }
        current = input[i] - '0';
        res = res * 10 + current;
        i++;
    }

    if (isNegative)
        res *= -1;

    return res;
}

// hex string to integer
unsigned long hex2int(char* hex, int n)
{
    unsigned long val = 0;
	for(int i = 0; i < n; i++)
	{
        // get current character then increment
        int byte1 = hex[i];
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte1 >= '0' && byte1 <= '9')
            byte1 = byte1 - '0';
        else if (byte1 >= 'a' && byte1 <= 'f')
            byte1 = byte1 - 'a' + 10;
        else if (byte1 >= 'A' && byte1 <= 'F')
            byte1 = byte1 - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = val * 16 + byte1;
    }
    return val;
}