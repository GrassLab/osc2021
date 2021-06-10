# include "my_math.h"

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
  unsigned long r;
  for (int i=0; i<l; i++){
    if (s[i] >= 'a') s[i] = s[i] -(char)32;
    if (s[i] >= 'A') r = r*16+( (unsigned long)s[i] - 55);
    else r = r*16+( (unsigned long)s[i] - 48);
  }
  return r;
}
