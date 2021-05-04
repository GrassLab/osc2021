#include "lib.h"
int main() {
  // Would result in a absolute address
  char *args[4];
  char *name = "./hello_world.out";
  args[0] = "./hello_world.out";
  args[1] = "ian";
  args[2] = "nctu";
  args[3] = NULL;
  exec(name, (const char **)args);
  return 0;
}