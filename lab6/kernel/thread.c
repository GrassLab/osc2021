#include "uart.h"
#include "util.h"
#include "buddy.h"
#include "allocator.h"
#include "cpio.h"
#include "thread.h"

struct thread *head, *tail;
int thread_count = 0;
struct dynamic_allocator *dyalloc = 0;

/*
*	init_thread
*   
*   initial thread parameter and memory parameter
*/
void init_thread()
{
    head = tail = 0;
    thread_count = 0;
	
	buddy_init();
	dyalloc = dynamic_allocator_init();
}

/*
*	thread_test (requirment1 Test method)
*   
*   this code form TA, create 3 thread excute foo
*   1. create first thread and set its current thread
*   2. create N thread excute foo, N should > 2 
*   3. idle
*/
void thread_test()
{
	// create first thread for context_switch
    struct thread *first_thread = thread_create(0);
	set_current(first_thread);
	
    for(int i = 0; i < 3; ++i)
	{
        thread_create(foo);
    }
	
    idle();
}

/*
*	foo (requirment1 Test method)
*   
*   this code form TA, create a thread excute foo
*   1. for loop 0-9
*   2. print message and call schedule to next thread
*   3. delay 1000000
*   4. call schedule switch to next running thread
*   5. when finish, call exit to end of a thread
*/
void foo()
{	
	char buf[16] = {0};
	
    for(int i = 0; i < 10; ++i)
	{
		// print message, ex. Thread id: 1, index:3
        uart_putstr("Thread id: ");
		unsignedlonglongToStr(current_thread()->tid, buf);
		uart_putstr(buf);
		uart_putstr(", index: ");
		unsignedlonglongToStr(i, buf);
		uart_putstr(buf);
		uart_putstr("\n");
		
        delay(1000000);
        schedule();
    }
	
	// when finish, call exit to end of a thread
	exit();
}

/*
*	thread_create (requirment Creating a Thread)
*   
*   1. create new thread
*      - allocate thread momoery
*      - fp = Frame pointer = start address + THREAD_SIZE
*      - lr = link register for function calls.
*      - sp = stack pointer = start address + THREAD_SIZE
*      - other parameter
*
*   2. add new thread to run queue
*   3. return new_thread for fist initinal
*/
struct thread* thread_create(void* func)
{
	// create new thread	
    struct thread* new_thread = (struct thread*)dynamic_alloc(dyalloc, THREAD_SIZE);	
    new_thread->context.fp = (unsigned long)new_thread + THREAD_SIZE;
    new_thread->context.lr = (unsigned long)func;
    new_thread->context.sp = (unsigned long)new_thread + THREAD_SIZE;
    new_thread->tid = thread_count++;
    new_thread->status = TASK_RUNNING;
    new_thread->next = 0;
	
	// for requirment 2
	new_thread->program_addr = 0;
	new_thread->program_size = 0;
	new_thread->childID = 0;
	
	// for lab6 requirment2
	for (int i = 0; i < FD_MAX; ++i) 
		new_thread->fd_table.files[i] = 0;
	
	// add new thread to run queue
    add_to_run_queue(new_thread);
	
    return new_thread;
}

/*
*	add_to_run_queue (requirment Creating a Thread)
*   
*   head : pointer to run queue start
*   tail : pointer to run queue end
*
*   add thread to tail->next
*/
void add_to_run_queue(struct thread* new_thread)
{
    if(head == 0)
        head = tail = new_thread;
    else
	{
        tail->next = new_thread;
        tail = new_thread;
    }
}

/*
*	current_thread
*   
*   call get_current method get cureent thread (context_switch.s)
*/
struct thread* current_thread()
{
    return get_current();
}

/*
*	schedule (requirment Scheduler and Context Switch)
*   
*   pointer to next thread and do context_switch
*   1. if head = 0 represent run queue is empty, return
*   2. if head = tail represent run queue only first thread, free first thread and init
*   3. if run queue thread count > 1,  pointer to next thread and do context switch
*
*/
void schedule()
{
    if(head == 0)  // run queue is empty
        return;
    
    if(head == tail && thread_count > 1) // run queue only first thread
	{
		dynamic_free(dyalloc, (unsigned long)head);
		head = tail = 0;
		thread_count = 0;
		return;
	}

	// pointer to next thread and do context switch
	//
	//							  head  _________		
	//								   |	     |
	//     head        tail            |         ▼
	//    _____ _____ _____          _____     _____ _____
	//   |     |     |     |        |     |   |     |     |       
	//   |     |     |     |   =>   |     | x |     |     | 
	//   -------------------        -------   -------------
	//                                ▲                |
	//                                |________________| tail
	do
	{
		tail->next = head;
		tail = head;
		head = head->next;
		tail->next = 0;
	} while(head->status != TASK_RUNNING);

	// put [current_thread] and [next_thread] do thread context switch
	switch_to(current_thread(), head);
}

/*
*	kill_zombies (requirment The Idle Thread)
*   
*   scan run queue, from head to tail. recycle not use(dead) thread.
*   1. ptr = current thread pointer
*   2. if ptr = 0 represent run queue is empty, return
*   3. if next thread status is dead, free next thread,
*      current thread's next pointer to ptr->next->next   
*   4. if next thread status isn't dead, ptr = ptr->next
*/
void kill_zombies()
{
    struct thread* ptr = head;
	struct thread* tmp = 0;
	
	if(ptr == 0)  // empty 
		return;
		
    while(1)
	{
		//							  ptr  ________________		
		//								   |	           |
		//     ptr                         |               ▼
		//    _____ _____ _____          _____   _____   _____
		//   |     |     |     |        |     | |     | |     |       
		//   |     |dead |     |   =>   |     | |dead | |     | 
		//   -------------------        -------  -----   -----
		//                                        free
		//                                		
        while(ptr->next != 0 && ptr->next->status == TASK_DEAD)
		{
            tmp = ptr->next->next;
			dynamic_free(dyalloc, (unsigned long)ptr->next);	
			ptr->next = tmp;
        }
		
		// if next thread status isn't dead, ptr = ptr->next
		if(ptr->next != 0)
		{
			ptr = ptr->next;
        }
        else
		{
            tail = ptr;
            break;
		}
	}
}

/*
*	idle (requirment The Idle Thread)
*   
*   this code form TA, always run kill_zombies and schedule
*/
void idle()
{
    while(1)
	{
        kill_zombies(); // reclaim threads marked as DEAD
		do_fork(); // for requirment 2
        schedule(); // switch to any other runnable thread
		
		if (head == 0 && tail == 0)
			return;
    }
}

void delay(int count)
{
	for(int i = 0; i < count; i++)
		asm volatile("nop");
}

/*
*	exit (requirment End of a Thread)
*   
*   When a thread finishes its jobs, set status to TASK_DEAD
*   and call schedule to next running thread
*/
void exit()
{
	struct thread *cur = current_thread();
	cur->status = TASK_DEAD;
	schedule();
}

//=======================================================================

#define USER_PROGRAM_BASE		0x5000000
#define USER_PROGRAM_ADDR		0x60000
int user_program_count = 0;

void init_thread2()
{
	user_program_count = 0;
}

/*
*	thread_test2 (requirment2 Test method)
*   
*   this code form TA, create thread excute user_test
*   1. create first thread and set its current thread
*   2. create thread excute user_test
*   3. idle
*/
void thread_test2()
{
    struct thread *first_thread = thread_create(0);
	set_current(first_thread);
	
    thread_create(user_test);
    idle();
}

/*
*	user_test (requirment2 Test method)
*   
*   this code form TA, Execute user program with arguments
*   1. Declare arguments
*   2. call exec function create thread and excute user program
*/
void user_test()
{
    char argv[4][10] = { "argv_test", "-o", "arg2", ""};	
    exec("argv_test.img", argv);
}

/*
*	exec (requirment2 Arguments Passing)
*   
*   Execute user program with arguments
*   1. Declare user program excute address,
       Because we not MMU, so differnet user program should use different address
*   2. load_program_with_args
*   3. when finish, call exit to end of a thread
*/
void exec(const char* name, char(*argv)[10])
{
    unsigned long addr = generate_user_addr();
    load_program_with_args(name, argv, addr, head);
	
	// when finish, call exit to end of a thread
    exit();
}

/*
*	generate_user_addr (requirment2 Arguments Passing)
*   
*   calculate user program loading address (different program run different address)
*   1. formula = USER_PROGRAM_BASE + user_program_count * USER_PROGRAM_ADDR
*   2. user_program_count + 1
*   3. return user program loading address
*/
unsigned long generate_user_addr()
{
    unsigned long addr = USER_PROGRAM_BASE + user_program_count * USER_PROGRAM_ADDR;
    user_program_count++;
    return addr;
}

/*
*	load_program_with_args (requirment2 Arguments Passing)
*   
*   call cpio load user program, and pass argument to stack, finally run user program on EL0
*   1. call cpio_load_user_program_and_get_size, load user program and return program size
*   2. call pass_argument , pass argument to stack
*   3. set program address, program size
*   4. run user program on EL0 (EL1 -> EL0)
*/
void load_program_with_args(const char* name, char(*argv)[10], unsigned long addr, struct thread* current)
{
	int program_size = cpio_load_user_program_and_get_size((char*)name, addr);
	
	unsigned long sp_addr = pass_argument(argv, addr);
    current->program_addr = addr;
    current->program_size = program_size;
	
	// run user program on EL0 (EL1 -> EL0)
	asm volatile("mov x0, 0x340			\n\t");   // 340 = 1101000000, irq enable
	asm volatile("msr spsr_el1, x0		\n\t");   // spsr_el1 = 0x340
	asm volatile("msr elr_el1, %0		\n\t"::"r"(addr));  // elr_el1 = user program load address
	asm volatile("msr sp_el0, %0		\n\t"::"r"(sp_addr)); // sp_el0 = stack address
	asm volatile("mrs x3, sp_el0		\n\t");  // 將sp_el0給x3 
    asm volatile("ldr x0, [x3, 0]		\n\t");  // x0存放(x3+0)，參考下圖，也就是argc的位置
    asm volatile("ldr x1, [x3, 8]		\n\t");  // x1存放(x3+8)，參考下圖，也就是argv的位置
	asm volatile("eret					\n");    
}

/*
*	pass_argument (requirment2 Arguments Passing)
*   
*   pass argument to user stack, and return user stack address (aligment 16)
*   1. calc argv(argc), and argv byte count
*   2. calc offset, notice sp should aligment 16
*   3. pass argument to user stack
*   4. return stack address
*/
unsigned long pass_argument(char(*argv)[10], unsigned long addr)
{
	/*  user stack   
	*
	*   Low ----------------------------------------------------------------------> High                         
	*                    ______________
	*   _________________|_____________▼______________________________________________
	*   |         |             |               |               |      |       |      |
	*   |         |             |               |               |      |       |      |
	*   | 2(argc) | char** argv | char* argv[0] | char* argv[1] | NULL | a.out | arg1 |
	*   |         |             |               |               |      |       |      |
	*   |_________|_____________|_______________|_______________|______|_______|______|
	*                                  |                |                  ▲       ▲  
	*                                  -------------------------------------       |
	*                                                   |__________________________|                                                                            
	*/
	
	// // 1.計算參數數量(argc), 以及參數各有幾個byte(argv byte count)
    // 例如{"argv_test", "-o", "arg2", ""}，共有4個參數，18個byte(含結束符號\0)
    int argc = 0, argv_count = 0;
    for(int i = 0;;++i)
	{
        ++argc;
        if(strlen(argv[i]) == 0) 
			break;
		
        for(int j = 0;; ++j)
		{
            ++argv_count;
            if(argv[i][j] == 0) 
				break;
        }
    }
    
	// 2. calc offset, notice sp should aligment 16
	// 參考上圖，第一個放argc，然後放argv的地址，接著放參數位址，然後放每一個參數的內容
	// 因此，會先計算參數所佔的大小是多少，然後載入位(addr)  – 參數所佔大小(offset)
    // 就是我們的user stack的位址，記得要對齊16
    int offset = (1 + 1 + argc) * 8 + argv_count;
    addr = addr - offset;
    addr = addr - (addr & 15); // aligment 16
	
	// 3. 將參數塞入user stack
    // 3.1 起始位置 = addr 	
    char* data = (char*)addr;
	// 3.2 塞入argc的數量，然後位址+8
    *(unsigned long*)data = argc - 1; // 不包含最後一個NULL
    data += 8;	
	// 3.3 塞入argv的地址後，位址+8
    *(unsigned long*)data = (unsigned long)(data + 8); //argv addr
    data += 8;
	// 3.4 位址 += 參數數量*8
    char* argv_buf = data + 8 * argc;
	// 3.5 塞入參數的內容
    for(int i = 0; i < argc - 1; ++i)
	{
        *(unsigned long*)data = (unsigned long)argv_buf;
        for(int j = 0;;++j)
		{
            *argv_buf = argv[i][j];
            ++argv_buf;
            if(argv[i][j] == 0) 
				break;
        }
		// 塞完後位址+8
        data += 8;
    }
	// 結束放0
    *(unsigned long*)data = 0;
	
    return addr;
}

/*
*	get_pid (requirment2)
*   
*	get current thread id
*/
unsigned long get_pid()
{
    return current_thread()->tid;
}

/*
*	fork (requirment2)
*   
*	1. set current thread status = fork
*   2. call schedule
*   3. return child id
*/
int fork()
{
    head->status |= TASK_FORK;
    schedule();
    return head->childID;
}

/*
*	do_fork (requirment2)
*   
*	1. from head to tail，search status = fork thread
*   2. create new thread
*   3. copy current thread to new thread
*/
void do_fork()
{
    for(struct thread* ptr = head->next; ptr != 0; ptr = ptr->next)
	{
        if((ptr->status) & TASK_FORK)
		{
            struct thread* child = thread_create(0);
            copy_program(ptr, child);
        }
    }
}

/*
*	copy_program (requirment2)
*
*   copy parent thread to new thread
*   1. set parent status and child id
*   2. copy parent thread to child thread
*   3. set
*   4. copy parent statck to child stack
*
*/
void copy_program(struct thread* parent, struct thread* child)
{
	// 1. 設定parent狀態設為非fork，並將child id給parent的childID
    parent->status ^= TASK_FORK;
    parent->childID = child->tid;
	
	// 2. 複製parent thread內容給child
    struct thread* child_next = child->next; //先將child next及id取出，以免被覆蓋
    unsigned long org_tid = child->tid;
	
	// 將parent thread內容複製給child
    char* src = (char*) parent;
    char* dst = (char*) child;
    for(int i = 0; i < THREAD_SIZE; ++i)
	{
        *dst = *src;
		src++;
		dst++;
	}
    
	// 設定child相關參數(並把剛剛的child next及id回填)
    child->next = child_next;
    child->childID = 0;
    child->tid = org_tid;
    child->program_addr = generate_user_addr();
    child->program_size = parent->program_size;
	
	// 3. 設定 fp, sp, elr_el1, sp_el0, user_fp, user_lr
	//    3.1 計算暫存器偏移 
	//    3.2 計算program偏移
	//    3.3 原本是parent的位置，應該加上偏移才是正確的值
	//        其中，fp sp是加上暫存器偏移 
	//        elr_el1, sp_el0, user_fp, user_lr是加上program偏移
    unsigned long reg_addr_diff = (unsigned long)child - (unsigned long)parent;
    unsigned long p_diff = (unsigned long)child->program_addr - (unsigned long)parent->program_addr;
    child->context.fp += reg_addr_diff; // fp
    child->context.sp += reg_addr_diff; // sp
    child->context.elr_el1 += p_diff; //elr_el1
    child->context.sp_el0 += p_diff; //sp_el0
    child->context.reg[29] += p_diff; //user_fp
    child->context.reg[30] += p_diff; //user_lr

	// 4. 計算好正確位置後，將parent stack內容複製給child stack
    src = (char*) (parent->context.sp_el0);
    dst = (char*) (child->context.sp_el0);
    unsigned long st_size = (parent->program_addr) + parent->program_size - parent->context.sp_el0;
    for(int i = 0; i < st_size; ++i)
	{
        *dst = *src;
		src++;
		dst++;
    }
}

//=======================================================================

/*
*	thread_vfs_req2 (lab6 requirment2 Test method)
*   
*   1. create first thread and set its current thread
*   2. create thread excute vfs_req2
*   3. idle
*/
void thread_vfs_req2() 
{
	struct thread *first_thread = thread_create(0);
	set_current(first_thread);
	thread_create(vfs_req2);
	idle();
}

/*
*	vfs_req2 (requirment2 Test method)
*   
*   1. Declare arguments
*   2. call exec function excute user program
*/
void vfs_req2()
{
	char argv[2][10] = { "vfs_test", ""};	
    exec("vfs_test.img", argv);
}

/*
*	thread_get_file (requirment2 Test method)
*   
*   輸入index，回傳目前thread的 file descriptor table中指定index的file
*/
struct file *thread_get_file(int fd) 
{
	struct thread *cur = current_thread();
	return cur->fd_table.files[fd];
}

/*
*	thread_register_fd (requirment2 Test method)
*   
*   傳入file，找目前thread file descriptor table中未使用的index加入，並回傳index
*/
int thread_register_fd(struct file *file) 
{
	if (file == 0) 
		return -1;
	
	struct thread *cur = current_thread();
	// find next available fd
	for (int fd = 0; fd < FD_MAX; ++fd) 
	{
		if (cur->fd_table.files[fd] == 0) 
		{
			cur->fd_table.files[fd] = file;
			return fd;
		}
	}
	
	return -1;
}

/*
*	thread_clear_fd (requirment2 Test method)
*   
*   輸入index，清空目前thread的 file descriptor table中指定index的內容
*   並回傳成功或失敗
*/
int thread_clear_fd(int fd) 
{
	if (fd < 0 || fd >= FD_MAX) 
		return -1;
	
	struct thread *cur = current_thread();
	cur->fd_table.files[fd] = 0;
	
	return 1;
}

//=======================================================================

/*
*	thread_vfs_ele1 (lab6 elective1 Test method)
*   
*   1. create first thread and set its current thread
*   2. create thread excute vfs_ele1
*   3. idle
*/
void thread_vfs_ele1() 
{
	struct thread *first_thread = thread_create(0);
	set_current(first_thread);
	thread_create(vfs_ele1);
	idle();
}

/*
*	vfs_ele1 (elective1 Test method)
*   
*   1. Declare arguments， list directory set current directory "."
*   2. call exec function excute user program
*/
void vfs_ele1()
{
	char argv[4][10] = { "ls_test", ".", "arg1", ""};
    exec("ls_test.img", argv);
}

//=======================================================================

/*
*	thread_vfs_ele2 (lab6 elective2 Test method)
*   
*   1. create first thread and set its current thread
*   2. create thread excute vfs_ele1
*   3. idle
*/
void thread_vfs_ele2() 
{
	struct thread *first_thread = thread_create(0);
	set_current(first_thread);
	thread_create(vfs_ele2);
	idle();
}

/*
*	vfs_ele2 (elective2 Test method)
*   
*   1. Declare arguments
*   2. call exec function excute user program
*/
void vfs_ele2()
{
	char argv[2][10] = { "multilvl", ""};
    exec("multilvl_test.img", argv);
}
