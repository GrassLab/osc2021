# ifndef BITSET
# define BITSET

# define INTBITS (sizeof(int)*8)

int bitset_get_first_one(int *p, int len);

int bitset_get_first_zero(int *p, int len);

int bitset_get(int *p, int n, int len);

void bitset_set(int *p, int n, int len);

void bitset_clr(int *p, int n, int len);

void bitset_clrall(int *p, int len);

void bitset_show(int *p, int len);
# endif
