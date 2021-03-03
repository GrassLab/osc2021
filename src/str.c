int strcmp(char *str1, char *str2)
{
    int result = 1;
    int i = 0;
    while(str1[i] != '\0' && str2[i] != '\0')
    {
        result = 0;
        if (str1[i] != str2[i]) {
            return 1;
        }

        i++;
    }

    return result;
}