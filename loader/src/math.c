int pow(int base, int exponent)
{
    int result = 1;
    for (; exponent > 0; exponent--)
    {
        result = result * base;
    }
    return result;
}