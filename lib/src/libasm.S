.global raise_int
raise_int:
    svc 0x80
    ret
/*
 * x0: fd
 * x1: source
 * x2: length
 */
.global read
read:
    mov x3, x0
    mov x0, #0
    svc 0x80
    ret

.global write
write:
    mov x3, x0
    mov x0, #1
    svc 0x80
    ret

.global nanosleep
nanosleep:
    mov x1, x0
    mov x0, #35
    svc 0x80
    ret

.global get_pid
get_pid:
    mov x0, #39
    svc 0x80
    ret

.global fork
fork:
    mov x0, #57
    svc 0x80
    ret

.global exec
exec:
    mov x2, x1
    mov x1, x0
    mov x0, #59
    svc 0x80
    ret

.global exit
exit:
    mov x0, #60
    svc 0x80
    ret

.global wait
wait:
    mov x0, #61
    svc 0x80
    ret

.global api_test
api_test:
    mov x0, #400
    svc 0x80
    ret

.global yield
yield:
    mov x0, #401
    svc 0x80
    ret

.global thread
thread:
    mov x1, x0
    mov x0, #402
    svc 0x80
    ret

.global stack_info
stack_info:
    mov x3, x2
    mov x2, x1
    mov x1, x0
    mov x0, #403
    svc 0x80
    ret
