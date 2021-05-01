#include "lib.h"
int main() {
  // Would result in a absolute address
  // char *args[3] = {"ian", "nctu", NULL};
  char *args[4];
  args[0] = "./hello_world.out";
  args[1] = "ian";
  args[2] = "nctu";
  args[3] = NULL;
  exec(args);
  return 0;
}