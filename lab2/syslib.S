#include "include/syscall.h"

.global get_pid
get_pid:
  mov x8, SYS_GET_PID
  svc 0
  ret

.global exec
exec:
  mov x8, SYS_EXEC
  svc 0
  ret

.global fork
fork:
  mov x8, SYS_FORK
  svc 0
  ret

.global exit
exit:
  mov x8, SYS_EXIT
  svc 0
  ret

.global uart_read
uart_read:
  mov x8, SYS_UART_READ
  svc 0
  ret

.global uart_write
uart_write:
  mov x8, SYS_UART_WRITE
  svc 0
  ret

.global sigreturn
sigreturn:
  mov x8, SYS_SIGRETURN
  svc 0
  ret

.global kill
kill:
  mov x8, SYS_KILL
  svc 0
  ret

.global signal
signal:
  mov x8, SYS_SIGNAL
  svc 0
  ret

.global useless
useless:
  mov x8, SYS_USELESS
  svc 0
  ret