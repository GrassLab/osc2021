.section ".text"

.global sys_schedule
sys_schedule:
    svc 0

.global sys_schedule
sys_uart_read:
    svc 1
