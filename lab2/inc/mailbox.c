#include "mailbox.h"
#include "mmio.h"

#define MBOX_REQUEST            0
#define MBOX_CH_PROP            8//CPU->GPU
#define MBOX_TAG_GETSERIAL      0x10004
#define MBOX_TAG_GETREVISION    0x00010002
#define MBOX_TAG_GETVCMEM       0x00010006
#define MBOX_TAG_LAST           0
#define TAG_REQUEST_CODE        0

#define VIDEOCORE_MBOX  (MMIO_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_EMPTY      0x40000000
#define MBOX_FULL       0x80000000
#define MBOX_RESPONSE   0x80000000


volatile unsigned int __attribute__((aligned(16))) mbox[36];//message buffer

int mbox_call(unsigned char ch){
	unsigned int r=(((unsigned long)mbox&~0xF) | (ch&0xF));//Combine the message address with channel number
	do{ asm volatile("nop"); }while(*MBOX_STATUS & MBOX_FULL);//Check if Mailbox 0 status register’s full flag is set.
	*MBOX_WRITE=r;//write to Mailbox 1 Read/Write register.
	while(1){
		do{ asm volatile("nop"); }while(*MBOX_STATUS & MBOX_EMPTY);//Check if Mailbox 0 status register’s empty flag is set.
		if(r==*MBOX_READ){//read from Mailbox 0 Read/Write register.Check if the value is the same as you wrote in step 1.
			return mbox[1]==MBOX_RESPONSE;//is it a valid successful response?
		}
	}
	return 0;
}

int getSerialNum(unsigned int* dst){
	mbox[0]=8*4;
	mbox[1]=MBOX_REQUEST;
	mbox[2]=MBOX_TAG_GETSERIAL;
	mbox[3]=8;
	mbox[4]=TAG_REQUEST_CODE;
	mbox[5]=0;
	mbox[6]=0;
	mbox[7]=MBOX_TAG_LAST;
	
	int success=mbox_call(MBOX_CH_PROP);
	if(success){
		dst[0]=mbox[5];
		dst[1]=mbox[6];
	}
	return success;
}

int getBoardRevision(unsigned int* dst){//it should be 0xa020d3 for rpi3 b+
	mbox[0]=7*4;
	mbox[1]=MBOX_REQUEST;
	mbox[2]=MBOX_TAG_GETREVISION;
	mbox[3]=4;
	mbox[4]=TAG_REQUEST_CODE;
	mbox[5]=0;
	mbox[6]=MBOX_TAG_LAST;

	int success=mbox_call(MBOX_CH_PROP);
	if(success){
		dst[0]=mbox[5];
	}
	return success;
}

int getVCMEM(unsigned int* dst){
	mbox[0]=8*4;
	mbox[1]=MBOX_REQUEST;
	mbox[2]=MBOX_TAG_GETVCMEM;
	mbox[3]=8;
	mbox[4]=TAG_REQUEST_CODE;
	mbox[5]=0;
	mbox[6]=0;
	mbox[7]=MBOX_TAG_LAST;

	int success=mbox_call(MBOX_CH_PROP);
	if(success){
		dst[0]=mbox[5];
		dst[1]=mbox[6];
	}
	return success;
}