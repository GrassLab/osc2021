#include "sched_test.h"
#include <sched.h>
#include <printf.h>
#include <timer.h>

void foo(){
  for(int i = 0; i < 10; ++i) {
    printf("Thread id: %d %d\n", get_current()->task_id, i);
    delay(1);
    //schedule();
  }
}