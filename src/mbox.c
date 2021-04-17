#include "mbox.h"

unsigned long arm_memory_start, arm_memory_end;

int mbox_call(unsigned int *mbox, unsigned char channel) {
    unsigned int r = (unsigned int)(((unsigned long)mbox & (~0xf)) | (channel & 0xf));
    while(*MBOX_STATUS & MBOX_FULL) {
    }
    *MBOX_WRITE = r;

    while(1) {
        while(*MBOX_STATUS & MBOX_EMPTY) {}

        if(r == *MBOX_READ) {
            return mbox[1] == MBOX_CODE_BUF_RES_SUCC;
        }
    }
    return 0;
}

void mbox_arm_memory() {
    unsigned int __attribute((aligned(16))) mbox[8];
    mbox[0] = 8 * 4;
    mbox[1] = MBOX_CODE_BUF_REQ;
    mbox[2] = MBOX_TAG_GET_ARM_MEMORY;
    mbox[3] = 8;
    mbox[4] = MBOX_CODE_TAG_REQ;
    mbox[5] = 0;
    mbox[6] = 0;
    mbox[7] = 0;

    mbox_call(mbox, 8);
    arm_memory_start = mbox[5];
    arm_memory_end = mbox[5] + mbox[6];
}
