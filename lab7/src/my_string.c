# include "my_string.h"
# include "uart.h"

int str_cmp(const char *s1, const char *s2){
  int i = 0;
  if (s1[0] == '\0' && s2[0] == '\0'){
    return 1;
  }
  while(s1[i]){
    if (s1[i] != s2[i]) return 0;
    i++;
  }

  if (s2[i] == '\0') return 1;
  return 0;
}

int str_len(const char *s){
  int r = 0;
  while(*s++){
    r++;
  }
  return r;
}

void str_copy(const char *src, char *target){
  while(1){
    *target = *src;
    if (*src){
      src++;
      target++;
    }
    else{
      break;
    }
  }
}

//https://yangacer.blogspot.com/2014/05/utf8-unicode-c_9750.html
int to_utf8(uint32_t unicode, char *utf8){
  uint8_t len = 0;
  uint8_t mask = 0xF0; // 1111 0000
  len =
    unicode < 0x10000 ? 
    unicode < 0x800 ? 
    unicode < 0x80 ?
    1 : 2 : 3 : 4 ;
  mask >>= 8 - len;
  mask <<= 8 - len;
  if (len == 1) mask = 0;
  utf8[len] = '\0';
  for (int i = len-1; i>=1; i--){
    utf8[i] = (unicode & 0x3f) | 0x80;
    unicode >>= 6;
  }
  utf8[0] = (uint8_t)(mask | unicode);
  return len;
}
