# include "my_string.h"
# include "uart.h"

int str_cmp(char *s1, char *s2){
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

int str_len(char *s){
  int r = 0;
  while(*s++){
    r++;
  }
  return r;
}

void str_cat(char *src, char *target){
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
