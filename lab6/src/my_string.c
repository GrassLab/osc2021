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
