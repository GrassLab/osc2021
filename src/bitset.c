# include "bitset.h"
# include "uart.h"

int bitset_get_first_one(int *p, int len){
  int nr = 0;
  int *pt = p;
  while (nr < len){
    if (*pt == 0){
      pt++;
      nr+=INTBITS;
    }
    else{
      break;
    }
  }
  if(nr >= len) return -1;
  int t = *pt;
  while(t%2 == 0){
    t/=2;
    nr++;
  }
  return (nr >= len) ? -1 : nr;
}

int bitset_get_first_zero(int *p, int len){
  int nr = 0;
  int *pt = p;
  while (nr < len){
    if (*pt == -1){
      pt++;
      nr+=INTBITS;
    }
    else{
      break;
    }
  }
  if(nr >= len) return -1;
  int t = *pt;
  while(t%2 == 1){
    t/=2;
    nr++;
  }
  return (nr >= len) ? -1 : nr;
}

int bitset_get(int *p, int n, int len){
  if(n >= len) return -1;
  int nr = p[n/INTBITS];
  int nc = n%INTBITS;
  return (nr >> nc)%2;
}

void bitset_set(int *p, int n, int len){
  if(n >= len) return ;
  int nr = p[n/INTBITS];
  int nc = n%INTBITS;
  nr |= (1 << nc);
  p[n/INTBITS] = nr;
}

void bitset_clr(int *p, int n, int len){
  if(n >= len) return ;
  int nr = p[n/INTBITS];
  int nc = n%INTBITS;
  nr &= ~(1 << nc);
  p[n/INTBITS] = nr;
}

void bitset_clrall(int *p, int len){
  int t = 0;
  while (t < len){
    *p = 0;
    p++;
    t += INTBITS;
  }
}

void bitset_show(int *p, int len){
  int t = 0;
  char ct[2];
  ct[1] = '\0';
  while (t < len){
    int pt = *p;
    ct[0] = ((pt >> (t%INTBITS)) % 2)?'1':'0';
    uart_puts(ct);
    if ((t+1)%4 == 0){
      uart_puts((char *) " ");
    }
    if ((t+1)%INTBITS == 0){
      p++;
    }
    t++;
  }
}

