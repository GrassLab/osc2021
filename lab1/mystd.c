#include "uart.h"

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