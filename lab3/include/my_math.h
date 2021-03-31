template <class T> T template_test(T a, T b);
void int_to_str(int n, char *s);
//void int_to_str(unsigned long n, char *s);
void int_to_hex(unsigned long n, char *s);
void int_to_hex_align(unsigned long long n, char *s, int len);
unsigned long hex_to_uint(char *s, int l);
int str_to_int(char *c);
int round_up(int n, int a);
int align_up(int n, int a);
