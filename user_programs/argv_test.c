#include "lib.h"
int main() {
  // Would result in a absolute address
  // char *args[3] = {"ian", "nctu", NULL};
  char *args[3];
  char *name = "./hello_world.out";
  args[0] = "ian";
  args[1] = "nctu";
  args[2] = NULL;
  exec(name, (const char **)args);
  return 0;
}