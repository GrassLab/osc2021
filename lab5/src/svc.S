
.global svc_get_core_timer_value
svc_get_core_timer_value:
  svc #1
  ret

.global svc_get_core_timer_ms
svc_get_core_timer_ms:
  svc #2
  ret

.global set_timeout_svccall
set_timeout_svccall:
  svc #5
  ret

.global getpid
getpid:
  svc #6
  ret

.global uart_read
uart_read:
  svc #7
  ret

.global uart_write
uart_write:
  svc #8
  ret

.global exec
exec:
  svc #9
  ret

.global exit
exit:
  svc #10
  ret

