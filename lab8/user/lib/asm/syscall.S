#include "syscall.h"
.section ".text"

.global getpid
getpid:
  mov  x8, GETPID
  svc  #0
  ret

.global uart_read
uart_read:
  mov  x8, UART_READ
  svc  #0
  ret

.global uart_write
uart_write:
  mov  x8, UART_WRITE
  svc  #0
  ret

.global exec
exec:
  mov  x8, EXEC
  svc  #0
  ret

.global exit
exit:
  mov  x8, EXIT
  svc  #0
  ret

.global fork
fork:
  mov  x8, FORK
  svc  #0
  ret  

.global get_time
get_time:
  mov x8, GET_TIME
  svc #0
  ret

.global open
open:
  mov x8, OPEN
  svc #0
  ret

.global close
close:
  mov x8, CLOSE
  svc #0
  ret

.global write
write:
  mov x8, WRITE
  svc #0
  ret
.global read
read:
  mov x8, READ
  svc #0
  ret