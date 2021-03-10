#ifndef __CPIO_H_
#define __CPIO_H_

void cpio_list_file(void *cpio_buf);
char *cpio_get_content(char *path, char *cpio_buf);

#endif