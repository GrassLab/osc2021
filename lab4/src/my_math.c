# include "my_math.h"

template <class T> T template_test(T a, T b){
  return a+b;
}

//itoa
void int_to_str(int n, char *s){
  char tmp[100];
  int idx = 0;
  do{
    tmp[idx] = (char)((n%10) + 48);
    idx++;
    n /= 10;
  } while(n > 0);
  for (int i=0; i<idx; i++){
    s[i] = tmp[idx-i-1];
  }
  s[idx] = '\0';
}
/*
void int_to_str(unsigned long n, char *s){
  char tmp[100];
  int idx = 0;
  do{
    tmp[idx] = (char)((n%10) + 48);
    idx++;
    n /= 10;
  } while(n > 0);
  for (int i=0; i<idx; i++){
    s[i] = tmp[idx-i-1];
  }
  s[idx] = '\0';
}
*/

void int_to_hex_align(unsigned long long n, char *s, int len){
  int idx = len;
  do{
    unsigned long long t = n%16;
    if (t < 10)
      s[idx] = (char)(t + 48);
    else
      s[idx] = (char)(t + 87);
    idx--;
    n /= 16;
  } while(n > 0);
  s[idx--] = 'x';
  s[idx--] = '0';
  while(idx >= 0){
    s[idx--] = ' ';
  }
}

void int_to_hex(unsigned long n, char *s){
  char tmp[100];
  int idx = 0;
  do{
    unsigned long t = n%16;
    if (t < 10)
      tmp[idx] = (char)(t + 48);
    else
      tmp[idx] = (char)(t + 87);
    idx++;
    n /= 16;
  } while(n > 0);
  tmp[idx++] = 'x';
  tmp[idx++] = '0';
  for (int i=0; i<idx; i++){
    s[i] = tmp[idx-i-1];
  }
  s[idx] = '\0';
}

unsigned long hex_to_uint(char *s, int l){
  unsigned long r = 0;
  for (int i=0; i<l; i++){
    if (s[i] >= 'a') s[i] = s[i] -(char)32;
    if (s[i] >= 'A') r = r*16+( (unsigned long)s[i] - 55);
    else r = r*16+( (unsigned long)s[i] - 48);
  }
  return r;
}

int str_to_int(char *c){
  int r = 0;
  while(*c){
    r = r*10+(*c)-'0';
    c++;
  }
  return r;
}

int round_up(int n, int a){
  int r = n/a;
  return (n%a) ? r+1 : r;
}

int align_up(int n, int a){
  if (n%a == 0) return n;
  return (n/a+1)*a;
}
