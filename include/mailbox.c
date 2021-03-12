#include "mmio.h"
#include "mailbox.h"
#include "uart.h"

/* mailbox tag */
#define GET_BOARD_REVISION  0x00010002
#define GET_VC_MEMORY 0x00010006

#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

/*
 * If there is no space to write or nothing to read, the relative bit will be
 * set.
 */
#define MAIL_FULL 0x80000000
#define MAIL_EMPTY 0x40000000

/*
 * 1. Mailbox_buffer needs 16 bytes alignment
 * 2. The mailbox interface has 28 bits (MSB) available for the value and
 *    4 bits (LSB) for the channel.
 *     - Mailbox 0 Read/Write: CPU read from GPU (channel 8)
 *     - Mailbox 1 Read/Write: CPU write to GPU
 *     - channel 8: Request from ARM for response by VC
 *     - channel 9: Request from VC for response by ARM
 *
 * 3. Buffer Contents:
 *     - buffer size in bytes (u32)
 *     - buffer request/response code (u32)
 *     - tag
 *         - tag identifier (u32)
 *         - tag buffer size in bytes (u32)
 *         - request/reponse code (u32)
 *         - value buffer (>u32)
 *     - end tag (u32)
 */
int mailbox_call (unsigned int *mailbox_buffer, unsigned char channel) {
    /* Combine the message address with channel number. */
    unsigned int message = (unsigned int)((unsigned long)mailbox_buffer & ~0xF) | (channel & 0xF);

    /* wait until Mailbox 0 is not full */
    while (*mmio(MAILBOX_STATUS) & MAIL_FULL) ;

    /* write to Mailbox 1 read/write register */
    *mmio(MAILBOX_WRITE) = message;

    /* check Mailbox 0 empty falg*/
    while ((*mmio(MAILBOX_STATUS) & MAIL_EMPTY)) ;

    unsigned int result = *mmio(MAILBOX_READ);
    if (result == message)
        return mailbox_buffer[1] == REQUEST_SUCCEED;

    return 0;
}

/* raspi 3b+ should be 0xa020d3 */
unsigned int get_board_revision () {
    unsigned int __attribute__((aligned(16))) mailbox_buffer[7];
    /* buffer size */
    mailbox_buffer[0] = 7 * 4;
    mailbox_buffer[1] = REQUEST_CODE;

    /* tag begin */
    mailbox_buffer[2] = GET_BOARD_REVISION;
    /* value buffer size */
    mailbox_buffer[3] = 4;
    mailbox_buffer[4] = TAG_REQUEST_CODE;
    /* vaule buffer size */
    mailbox_buffer[5] = 0;

    mailbox_buffer[6] = END_TAG;

    if (mailbox_call(mailbox_buffer, 8))
        return mailbox_buffer[5];
    return 0;
}

int get_vc_memory (unsigned int *base, unsigned int *size) {
    unsigned int __attribute__((aligned(16))) mailbox_buffer[8];
    mailbox_buffer[0] = 8 * 4;
    mailbox_buffer[1] = REQUEST_CODE;

    /* tag */
    mailbox_buffer[2] = GET_VC_MEMORY;
    mailbox_buffer[3] = 8;
    mailbox_buffer[4] = TAG_REQUEST_CODE;
    mailbox_buffer[5] = 0;
    mailbox_buffer[6] = 0;

    mailbox_buffer[7] = END_TAG;

    int flag = mailbox_call(mailbox_buffer, 8);
    *base = mailbox_buffer[5];
    *size = mailbox_buffer[6];
    return flag;
}
