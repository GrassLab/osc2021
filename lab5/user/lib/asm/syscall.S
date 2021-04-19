#include "syscall.h"
.section ".text"

.global getpid
getpid:
  mov  x0, GETPID
  svc  #0
  ret

.global uart_read
uart_read:
  mov  x0, UART_READ
  svc  #0
  ret

.global uart_write
uart_write:
  mov  x0, UART_WRITE
  svc  #0
  ret

.global exec
exec:
  mov  x0, EXEC
  svc  #0
  ret

.global exit
exit:
  mov  x0, EXIT
  svc  #0
  ret
.global fork
fork:
  mov  x0, FORK
  svc  #0
  ret  
