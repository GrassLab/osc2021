#include "include/mini_uart.h"
#include "include/mm.h"
#include "include/cutils.h"
#include "utils.h"
#include "include/test.h"

int _test_buddy_system(void)
{
    struct page *A, *B, *C, *D;
    uart_send_string("================= Start =====================\r\n");
    show_mm();
    uart_send_string("=================alloc A=====================\r\n");
    A = get_free_frames(1);
    show_mm();
    uart_send_string("=================alloc B=====================\r\n");
    B = get_free_frames(2);
    show_mm();
    uart_send_string("=================alloc C=====================\r\n");
    C = get_free_frames(1);
    show_mm();
    uart_send_string("=================alloc D=====================\r\n");
    D = get_free_frames(2);
    show_mm();

    uart_send_string("=================free B=====================\r\n");
    free_frames(B);
    show_mm();
    uart_send_string("=================free D=====================\r\n");
    free_frames(D);
    show_mm();
    uart_send_string("=================free A=====================\r\n");
    free_frames(A);
    show_mm();
    uart_send_string("=================free C=====================\r\n");
    free_frames(C);
    show_mm();

    return 0;
}

int _test_kmalloc_system(void)
{
    char *A, *B, *C, *D, *E, *F, *G, *H, *I, *J;
    int num = 25;
    // show_mm();
uart_send_string("========== alloc A ==============\r\n");
    A = kmalloc(num);
    for (int i = 0; i < num; ++i)
        A[i] = '0' + i;
uart_send_string("========== alloc B ==============\r\n");
    B = kmalloc(num);
uart_send_string("========== alloc C ==============\r\n");
    C = kmalloc(num);
uart_send_string("========== alloc DEFGHI ==============\r\n");
    D = kmalloc(24);
    E = kmalloc(24);
    F = kmalloc(24);
    G = kmalloc(24);
    H = kmalloc(24);
    I = kmalloc(24);
    J = kmalloc(12287);
    // J = kmalloc(2048);
    for (int i = 0; i < 100; ++i)
        J[i] = '0' + i;
    uart_send_ulong((unsigned long)J);
    show_mm();

uart_send_string("========== free B ==============\r\n");
    kfree(B);

uart_send_string("========== free C ==============\r\n");

    kfree(C);
uart_send_string("========== free A ==============\r\n");
    kfree(A);
uart_send_string("========== free DEFGHI ==============\r\n");
    kfree(D);
    kfree(E);
    kfree(F);
    kfree(G);
    kfree(H);
    kfree(I);
    kfree(J);
    show_mm();
    return 0;
}