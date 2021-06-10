# include "user_lib.h"

const char *argv_test_argv[] = {"argv_test", "-o", "argv2", "argv3", "Hello~~~", 0};

void main(){
  chdir("/cpio/demo");
  exec("argv_test.img", (char **)argv_test_argv);
  exit();
}
