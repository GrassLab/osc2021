#include "lib.h"
int main(int argc, char **argv) {
  printf("argc: %d\n", argc);
  printf("argv: %x\n", argv);
  // int i = 0;
  // int i = 0;
  for (int i = 0; i < argc; i++) {
    printf("  argv[%d] = %s\n", i, argv[i]);
  }
  // while (1) {
  //   ;
  // }
  printf("Hello %s: %d!\n", "ian", 123);
  // while (1) {
  //   ;
  // }
  return 0;
}