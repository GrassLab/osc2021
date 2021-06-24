#include "start.h"

void main(int argc, char** argv) {
  int fd = open(argv[1], 0);
  dirent d;
  if(fd > 0) {
    int i = 0;
    while (1) {
      if(getdent(fd, i, &d) < 0) {
        break;
      }
      print("Name: ");
      print(d.name);
      print(" Size: ");
      print_hex(d.size);
      print("\n");
      i++;
    }
    
  }
}