#include "mmio.h"
#include "mailbox.h"

#define GET_BOARD_REVISION  0x00010002
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
    return result == message;
}

/* raspi 3b+ should be 0xa020d3 */
unsigned int get_board_revision () {
    unsigned int __attribute__((aligned(16))) mailbox_buffer[7];
    /* buffer size */
    mailbox_buffer[0] = 7 * 4;
    mailbox_buffer[1] = REQUEST_CODE;

    /* tag begin */
    mailbox_buffer[2] = GET_BOARD_REVISION;
    /* maximum of request and reponse value buffer's length */
    mailbox_buffer[3] = 4;
    mailbox_buffer[4] = TAG_REQUEST_CODE;
    /* vaule buffer */
    mailbox_buffer[5] = 0;
    mailbox_buffer[6] = END_TAG;

    if (mailbox_call(mailbox_buffer, 8))
        return mailbox_buffer[5];
    return 0;
}
