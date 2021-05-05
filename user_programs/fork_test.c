#include "lib.h"

int main(int argc, char **argv) {
  int result = fork();
  printf("fork test: %d\n", result);
}