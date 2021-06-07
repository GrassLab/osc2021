# include "load_app.h"
# include "uart.h"
# include "my_math.h"
# include "vfs.h"
# include "page.h"
# include "mem_addr.h"
# include "mem.h"
# include "flags.h"
# include "fat32.h"

//extern unsigned char _exec_app_img;

int load_app(char* pathname, uint64_t ttbr0){
  int fd = do_open(pathname, O_RD);
  if (fd < 0) return -1;
  uint64_t v_addr = USER_PRO_LR_START;
  int readbytes;
  do{
    uint64_t read_addr = create_user_page(v_addr, ttbr0);
    char *readbuf = (char*) (read_addr | KVA);
    readbytes = do_read(fd, readbuf, SECTOR_SIZE);
    char ct[20];
    int_to_str(readbytes, ct);
    uart_puts(ct);
    uart_puts("\n");
    v_addr += SECTOR_SIZE;
  } while(readbytes == SECTOR_SIZE);
  create_user_page(USER_PRO_SP_START, ttbr0);
  return 0;
}
