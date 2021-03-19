#ifndef MBOX_H
#define MBOX_H
#include <types.h>
#include <mm.h>
/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_GET_BOARD_REVISION 0x00010002

/* mailbox registers */
#define MBOX_BASE (MMIO_BASE + 0xb880)
#define MBOX_READ ((volatile unsigned int*)(MBOX_BASE))
#define MBOX_STATUS ((volatile unsigned int*)(MBOX_BASE + 0x18))
#define MBOX_WRITE ((volatile unsigned int*)(MBOX_BASE + 0X20))

#define MBOX_FULL 0X80000000
#define MBOX_EMPTY 0x40000000

/* mailbox message buffer */
volatile uint32_t __attribute__ ((aligned (16))) mbox[36];

#define MBOX_REQUEST_CODE 0x00000000
#define MBOX_REQUEST_SUCCEED 0x80000000
#define MBOX_REQUEST_FAILED 0x80000001
#define MBOX_TAG_REQUEST_CODE 0x00000000
#define MBOX_END_TAG 0x00000000
#define MBOX_RESPONSE 0x80000000
void get_board_revision();
int mbox_call(unsigned char ch);
#endif
