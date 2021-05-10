#ifndef __FS_H_
#define __FS_H_

typedef unsigned int fmode_t;
struct file {
  void *data;
  unsigned int size;
  fmode_t f_mode;
};

#endif