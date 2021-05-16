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
