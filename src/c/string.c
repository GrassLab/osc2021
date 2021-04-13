#include "string.h"
#include "math.h"

int strcmp(char *s1, char *s2)
{
    int i;

    for (i = 0; i < strlen(s1); i++)
    {
        if (s1[i] != s2[i])
        {
            return s1[i] - s2[i];
        }
    }

    return s1[i] - s2[i];
}

void strset(char *s1, int c, int size)
{
    int i;

    for (i = 0; i < size; i++)
        s1[i] = c;
}

int strlen(char *s)
{
    int i = 0;
    while (1)
    {
        if (*(s + i) == '\0')
            break;
        i++;
    }

    return i;
}

// https://www.geeksforgeeks.org/convert-floating-point-number-string/
void itoa(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    str[i] = '\0';
    reverse(str);
}

// https://www.geeksforgeeks.org/convert-floating-point-number-string/
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    itoa(ipart, res, 0);
    int i = strlen(res);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        itoa((int)fpart, res + i + 1, afterpoint);
    }
}

// https://www.geeksforgeeks.org/write-your-own-atoi/
int atoi(char *s)
{
    // Initialize result
    int value = 0;

    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corosponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total
    for (int i = 0; s[i] != '\0'; i++)
        value = value * 10 + s[i] - '0';

    // return result.
    return value;
}

void reverse(char *s)
{
    int i;
    char temp;

    for (i = 0; i < strlen(s) / 2; i++)
    {
        temp = s[strlen(s) - i - 1];
        s[strlen(s) - i - 1] = s[0];
        s[0] = temp;
    }
}

void strcpy(char *source, char *target)
{
    int length = strlen(source);

    for (int i = 0; i < length; i++)
        target[i] = source[i];
    target[length] = '\0';
}