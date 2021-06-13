#include "string.h"
#define BITS 64

/**
* @desc Algorithm to convert decimal to hexadecimal.
* 
* Compile: gcc decimal-hexa.c -o toHex
* Run: ./toHex 250
*
* @author Samuel T. C. Santos
*/


void strrev(unsigned char str[]){
	int len = strlen(str);
	unsigned char tmp, i, j;
	//int middle = len /2;
	
	for(i=0, j=len-1; i < j; i++,j--){
		tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}
}

void dec_hex(unsigned long number, unsigned char *hexadecimal){
	char HEX_VALUES[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	for(int i=0;i<20;i++)
		hexadecimal[i] = 0;
	
	//Need 65 because the last is '\0'
	//char hexadecimal[BITS + 1];
 
	//r - remainder , q - quotient
	int q=number ,r=0, index=0;
	if(q == 0){
		hexadecimal[index] = HEX_VALUES[0];
		index++;
	} 
	while (q != 0){
		r = number % 16;
		q = number / 16;
		hexadecimal[index] = HEX_VALUES[r];
		number = q;
		index++;
	}

	hexadecimal[index] = '\0';
	strrev(hexadecimal);
	
} 