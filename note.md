# Lab 3 : Allocator

kernel 需要提供分配記憶體的功能讓 user program 使用，若沒有 memory allocator，則使用者必須靜態地將記憶體切割成數個 memory pools 來存放不同大小的物件。通用的作業系統應該要能在 runtime 決定 physical memory 該如何分配，所以需要 dynamic memory allocation。

### Requirements
1. Buddy System
2. Dynamic Memory Allocator

### Elective
1. Startup Allocator

## Background
### Reserved Memory
在樹莓派 booting 後，有一些 physical memory 是已經被佔用的。例如：``0x0000`` ~ ``0x1000`` 放著 multicore boot 的 spin tables，此外，flatten device tree、initramfs、kernel image 都會佔用 physical memory。因此，我們實作的 memory allocator 需要避開這些已經被佔用的記憶體。

### Dynamic Memory Allocator
給定需要分配的記憶體大小，Dynamic Memory Allocator 需要在記憶體中找到足夠大的連續空間並返回其位址，此外，在使用後也應該釋放掉該記憶體區塊。

### Page Frame Allocator
若想要讓 user program 在 virtual memory 上執行，則需要劃分一些以 4KB memory 對齊的 4KB 的 memory block，稱為 page frames。這是因為 4KB 為 virtual memory mapping 的單位，所以，需要將可用的記憶體表示為 page frame。Page Frame Allocator 會由 Page Frame Array 管理所有的 Page Frame，此外，Page Frame Allocator 在分配 Page Frame 時應該盡量分配連續的 Page Frame。

**在 Requirement 中，需要規劃 physical address ``0x1000_0000 ~ 0x2000_0000`` 的記憶體空間。**

## Buddy System
Buddy System 是一種簡易的分配連續記憶體的演算法，雖然它有 internal fragmentation 的問題，然而，它很適合用於 page frame allocation 因為 fragmentation 的問題可以透過 dynamic memory allocator 來減緩。

### Data Structure
+ The Frame Array
	+ 這個陣列表示目前 memory 的分配狀態，它是由 physical memory frame 與陣列 entries 1 對 1 所組成的，例如，假設可使用的記憶體大小為 200KB 且 frame 大小為 4KB，則該陣列會有 50 個 entries，其中第一個表示開始位置為 ``0x0`` 的 frame。
	+ 陣列中的每一個 entry 都包含 ``idx`` 與 ``order``。
		+ ``order >= 0``：表示第 ``idx`` 的 frame 是可以分配的，該連續記憶體的大小為 $2^{val}*4KB$。
		+ ``order = <F> (user defined value)``：表示第 ``idx`` 的 frame 是可以分配的。
		+ ``order = <X> (user defined value)``：表示第 ``idx`` 的 frame 已經分配過了。
		+ ![](https://i.imgur.com/mBLT0tJ.png)
	+ Below is the generalized view of The Frame Array
		+ ![](https://i.imgur.com/2ljYM2F.png)
	+ 可以由以下公式計算連續記憶體的大小
		+ $block's\ physical\ address=block's\ index*4096+base\ address$
		+ $block's\ size=4096*2^{block's\ exponent}$
		```c=
        #define FREE_FRAME_ALLOCATABLE -1
        #define USED_FRAME_UNALLOCATABLE -2

		#define BASE_ADDRESS 0x10000000
        #define FRAME_SIZE 0x1000
        #define FRAME_NUMBERS 0x10000
        
        struct buddy_frame {
    		int idx;
    		int order;
    		struct buddy_frame *next;
		};
        
        struct buddy_frame the_frame_array[FRAME_NUMBERS];
		```
+ Linked-lists for blocks with different size
	+ 可以設置連續記憶體的最大 size 並建立一個紀錄各種 size 的 linked list。
	+ linked list 會紀錄各種 size 的可用記憶體空間，buddy allocator 會搜尋該 list 以找尋適當大小的記憶體空間，若對應大小的 list 為空，則會嘗試尋找更大的 block list。
	+ ![](https://i.imgur.com/qZlA4Zm.png)
+ Release redundant memory block
	+ 以上方法可能會分配一個比實際需求還要大的記憶體空間，因此，allocator 應該要可以切除不需要的部分並將不需要的部分放回 buddy system。
    ```c=
    #define FRAME_MAX_ORDER 16
    
	struct buddy_frame *frame_freelist[FRAME_MAX_ORDER];
    ```

### Free and Coalesce Blocks
為了讓 Buddy system 存放更大的連續記憶體，因此，當使用者釋放掉記憶體時，buddy allocator 不應該單純的將它放回 linked list，而是嘗試 Find the buddy 與 Merge iteratively。

+ Find the buddy
	+ 可以用 block's index 與 block's exponent 做 xor 運算以找到其 buddy，若 buddy 位於 page frame array 中，則可以將它們 merge 以形成更大的 block。
+ Merge iteratively
	+ 有一種可能性為 merge 完的 block 還有可以 merge 的 buddy，因此要用同樣的方式來尋找 merge block 的 buddy。
	+ 若無法找到 merge block 的 buddy 或是 merge block 的大小已經達到最大值，則 allocator 才會停止 merge 並將該 block 放回 linked list 中。


## Dynamic Memory Allocator
Requirement 1 的 page frame allocator 可以進行連續記憶體的分配，而 Dynamic memory allocator 僅需將 page frame 轉換成 physical memory address。

為了分配較小塊的記憶體，可以建立數個 memory pool 來存放一些常用的大小，如: 16、32、48、96...，接著，將 page frame 切分成數個 chunk。當需要分配記憶體時，則分配符合大小要求且未被分配的 slot；若沒有適合的 slot 可以分配，則從 page allocator 分配一個新的 page frame 並回傳一個 chunk 給 caller。

相同 page frame 的物件在位址上都會有相同的 prefix，allocator 可以在釋放掉 chunk 時，由此來判斷該 chunk 是屬於哪一個 memory pool。

## Startup Allocator

