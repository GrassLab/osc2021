#include "allocator.h"
#define debug 1
#if debug
#include "uart.h"
#endif

#define A_BASE 0x10000000
#define A_SIZE 0x10000000
#define F_SIZE (1<<12)
#define F_NUM (A_SIZE/F_SIZE)
//A_SIZE=0.2G

static int frame_table[F_NUM*2]={0,1,};
static unsigned long addr_table[F_NUM*2];
static int index_table[F_NUM];

unsigned long toAddr(int i){//id!=0
	unsigned long ret=addr_table[i];
	#if debug
		uart_printf("[0x%x] => (0x%x)\n",i,ret);
	#endif
	return ret;
}

int toIndex(unsigned long addr){
	int ret=index_table[(addr-A_BASE)/F_SIZE];
	#if debug
		uart_printf("(0x%x) => [0x%x]\n",addr,ret);
	#endif
	return ret;
}

void reclaimFrame(int i){
	frame_table[i]=1;
	int p=i;
	while(p/2){
		p=p/2;
		if(frame_table[p*2]&&frame_table[p*2+1]){
			frame_table[p*2]=frame_table[p*2+1]=0;
			frame_table[p]=1;
			#if debug
			uart_printf("-[0x%x]&[0x%x] is merged to [0x%x].\n",p*2,p*2+1,p);
			#endif
		}else{
			break;
		}
	}
	#if debug
	uart_printf("[0x%x] has been freed.\n",i);
	#endif
}

int getFrame(int i,int tar_size,int cur_size){
	frame_table[i]=0;
	if(cur_size==F_SIZE||tar_size*2>cur_size)return i;
	frame_table[i*2]=frame_table[i*2+1]=1;
	#if debug
	uart_printf("-[0x%x] is partitioned to [0x%x]&[0x%x].\n",i,i*2,i*2+1);
	#endif
	return getFrame(i*2,tar_size,cur_size/2);
}

int findFrame(int i,int tar_size,int cur_size,int offset){
	if(cur_size<F_SIZE||tar_size>cur_size)return 0;
	if(frame_table[i]){
		//do some cutting
		i=getFrame(i,tar_size,cur_size);
		addr_table[i]=A_BASE+offset;
		index_table[(addr_table[i]-A_BASE)/F_SIZE]=i;
		#if debug
		uart_printf("[0x%x] has been allocated.\n",i);
		#endif
		return i;
	}
	int ret=findFrame(i*2,tar_size,cur_size/2,offset);//left tree
	if(ret)return ret;
	return findFrame(i*2+1,tar_size,cur_size/2,offset+cur_size/2);//right tree
}

void* falloc(int size){//raspi 3 b+ have 1G RAM
	/*int id1=findFrame(1,A_SIZE/4,A_SIZE);
	toIndex(toAddr(id1));
	int id2=findFrame(1,A_SIZE/4,A_SIZE);
	toIndex(toAddr(id2));
	int id3=findFrame(1,A_SIZE/4,A_SIZE);
	toIndex(toAddr(id3));
	int id4=findFrame(1,A_SIZE/4,A_SIZE);
	toIndex(toAddr(id4));
	reclaimFrame(id1);
	reclaimFrame(id4);
	reclaimFrame(id2);
	reclaimFrame(id3);*/
	return (void*)toAddr(findFrame(1,size,A_SIZE,0));
}

void ffree(unsigned long addr){
	reclaimFrame(toIndex(addr));
}