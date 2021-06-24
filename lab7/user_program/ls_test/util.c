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

// compare two string is equal ?
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

void strReverse(char *buf, int size)
{
	int front = 0;
	int last = size;

	while(last > front)
	{
		char temp;
		temp = buf[front];
		buf[front] = buf[last];
		buf[last] = temp;

		front++;
		last--;
	}
}

void unsignedlonglongToStr(unsigned long long num, char *buf)
{
	if(num==0)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	int size=0;
	
	while(num)
	{
		buf[size] = '0' + num % 10;
		size++;
		num = num / 10;
	}

	buf[size] = '\0';
	strReverse(buf, size-1);

	return;
}

void unsignedlonglongToStrHex(unsigned long long num, char *buf)
{
	unsigned int n;
    int size=0;
    for(int c=60; c>=0; c-=4,size++) 
	{
        // get highest tetrad
        n=(num>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        buf[size] = (char)n;
    }
    buf[16] = '\0';
    
}

int log2(int input)
{
	int num = 1;
	int power = 0;

	while (num != input)
	{
		num = num << 1;
		power++;
	}

	return power;
}