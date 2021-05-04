#define EL0_SYNC_EXIT 0
#define EL0_SYNC_UART_SEND 1
#define EL0_SYNC_FORK 2
#define EL0_SYNC_PID 3
#define EL0_SYNC_EXEC 4
#define EL0_SYNC_UART_GETC 5
#define EL0_SYNC_UART_PUTS 6
#define EL0_SYNC_TEST 20
#define EL0_SYNC_PRINT_H 21

.global exit
exit:
  svc EL0_SYNC_EXIT
  b exit

.global _uart_send
_uart_send:
  svc EL0_SYNC_UART_SEND
  ret

.global _uart_puts
_uart_puts:
  svc EL0_SYNC_UART_PUTS
  ret

.global _uart_getc
_uart_getc:
  svc EL0_SYNC_UART_GETC
  ret

.global fork
fork:
  svc EL0_SYNC_FORK
  ret

.global test
test:
  svc EL0_SYNC_TEST
  ret

.global getpid
getpid:
  svc EL0_SYNC_PID
  ret

.global print_hex
print_hex:
  svc EL0_SYNC_PRINT_H
  ret

.global exec
exec:
  svc EL0_SYNC_EXEC
  ret