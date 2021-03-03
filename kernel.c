int uart_init() {
    return 0;
}

void kernel() {
    if (!uart_init()) {
    }
}
