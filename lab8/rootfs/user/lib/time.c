#include "time.h"
#include <syscall.h>
void delay(size_t sec) {
  size_t start_t, now_t;
  start_t = get_time();
  now_t = start_t;
  
  while((now_t - start_t) < 1000 * sec) {
    now_t = get_time();
  }
}