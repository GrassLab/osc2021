#include "allocator.h"
#define debug 0
#if debug
#include "uart.h"
#endif

#define A_BASE 0x10000000
#define A_SIZE 0x10000000
#define F_SIZE (1<<12)
#define F_NUM (A_SIZE/F_SIZE)
#define TLEVEL 20
//A_SIZE=0.2G

typedef struct{
	int beg,end;
	int data[F_NUM+1];//+1 for end
}List;

int listInsert(List* l,int v){
	int ret=l->end;
	l->data[ret]=v;
	l->end=(l->end+1)%(F_NUM+1);
	return ret;//insert position
}

int listGet(List* l,int i){
	int ret=l->data[i];
	l->data[i]=l->data[l->beg];
	l->beg=(l->beg+1)%(F_NUM+1);
	return ret;//item value
}

static List lists[TLEVEL];
static int frame_table[F_NUM*2];//record position of list
static unsigned long i2addr[F_NUM*2];
static int addr2i[F_NUM];

int __lg(int v){
	int ret=-1;
	while(v){
		++ret;
		v>>=1;
	}
	return ret;
}

unsigned long __toAddr(int i){//id!=0
	int root=31;
	while(1){
		if(i&(1<<root)){
			--root;
			break;
		}
		--root;
	}
	unsigned long ret=0;
	unsigned long delta=A_SIZE/2;
	while(root>=0){
		if(i&(1<<root))ret+=delta;
		--root;
		delta/=2;
	}
	return ret+A_BASE;
}

unsigned long toAddr(int i){
	unsigned long ret=i2addr[i];

	#if debug
	uart_printf("[0x%x] => (0x%x)\n",i,ret);
	#endif

	return ret;
}

int toIndex(unsigned long addr){
	int ret=addr2i[(addr-A_BASE)/F_SIZE];

	#if debug
	uart_printf("(0x%x) => [0x%x]\n",addr,ret);
	#endif

	return ret;
}

void reclaimFrame(int i){
	#if debug
	uart_printf("[0x%x] has been freed.\n",i);
	#endif

	int p=i;
	int level=__lg(p);
	while(frame_table[p^1]>=0){
		listGet(&lists[level],frame_table[p^1]);
		frame_table[p^1]=-1;

		#if debug
		uart_printf("-[0x%x]&[0x%x] is merged to [0x%x].\n",p,p^1,p/2);
		#endif

		p=p/2;
		--level;
	}

	frame_table[p]=listInsert(&lists[level],p);
}

int getFrame(int i,int tar_size,int cur_size){
	int level=__lg(i);
	while(1){
		if(cur_size==F_SIZE||tar_size*2>cur_size)return i;
		frame_table[i*2+1]=listInsert(&lists[level+1],i*2+1);

		i*=2;
		cur_size/=2;
		level++;

		#if debug
		uart_printf("-[0x%x] is partitioned to [0x%x]&[0x%x].\n",i/2,i,i^1);
		#endif
	}
}

int findFrame(int tar_size){
	for(int i=TLEVEL-1;i>=0;--i){
		int cur_size=A_SIZE>>i;
		int beg=lists[i].beg;
		int end=lists[i].end;
		if(cur_size>=F_SIZE&&cur_size>=tar_size&&beg!=end){
			int ret=listGet(&lists[i],beg);
			frame_table[ret]=-1;

			ret=getFrame(ret,tar_size,cur_size);
			i2addr[ret]=__toAddr(ret);
			addr2i[(i2addr[ret]-A_BASE)/F_SIZE]=ret;

			#if debug
			uart_printf("[0x%x] has been allocated.\n",ret);
			#endif

			return ret;
		}
	}

	#if debug
	uart_printf("Can't find a proper space.\n");
	#endif
	return 0;
}

void* falloc(int size){//raspi 3 b+ have 1G RAM
	return (void*)toAddr(findFrame(size));
}

void ffree(unsigned long addr){
	reclaimFrame(toIndex(addr));
}

/*------------------------------------------*/

typedef struct _Pool{
	int size;
	struct _Pool* next;
	long mask[4];
}Pool;

int reclaimChunk(Pool* pool,unsigned long addr){
	unsigned long tmp=(unsigned long)pool;
	if(addr>=tmp&&addr<tmp+F_SIZE){
		int index=(addr-tmp-sizeof(Pool))/pool->size;
		pool->mask[index/64]^=1<<(index%64);

		#if debug
		uart_printf("(0x%x) has been freed.\n",addr);
		#endif

		return 1;
	}
	return 0;
}

void* findChunk(Pool* pool){
	for(int i=0;i<4;++i){
		long mask=pool->mask[i];
		long lowbit=mask&-mask;
		if(lowbit){//exist space
			long offset=0;
			for(int j=0;((1<<j)&lowbit)==0;++j)offset+=1;
			if(sizeof(Pool)+(64*i+offset+1)*pool->size <= F_SIZE){//not exeed current frame
				pool->mask[i]^=1<<offset;
				unsigned long ret=(unsigned long)(pool+1)+(64*i+offset)*pool->size;

				#if debug
				uart_printf("(0x%x) has been allocated.\n",ret);
				#endif

				return (void*)ret;
			}else{
				break;
			}
		}
	}

	//no space
	if(!pool->next){
		Pool* tmp=(Pool*)falloc(F_SIZE);
		tmp->size=pool->size;
		tmp->next=0;
		for(int i=0;i<4;++i){
			tmp->mask[i]=-1;
		}
		pool->next=tmp;
	}
	return findChunk(pool->next);
}

Pool* pools[4];//12, 32, 48, ...

void* dalloc(int size){
	for(int i=0;i<4;++i){
		if(size<=pools[i]->size){
			return findChunk(pools[i]);
		}
	}
	return falloc(size);
}

void dfree(unsigned long addr){
	if((addr-A_BASE)%F_SIZE){
		reclaimChunk((Pool*)(addr-(addr-A_BASE)%F_SIZE),addr);
	}else{
		ffree(addr);
	}
}

/*------------------------------------------*/

void allocator_init(){
	frame_table[0]=-1;
	frame_table[1]=listInsert(&lists[0],1);
	for(int i=2;i<F_NUM*2;++i)frame_table[i]=-1;

	int chunk_size[4]={12,32,48,1024};
	for(int i=0;i<4;++i){
		pools[i]=(Pool*)falloc(F_SIZE);
		pools[i]->size=chunk_size[i];
		pools[i]->next=0;
		for(int j=0;j<4;++j){
			pools[i]->mask[j]=-1;
		}
	}
}