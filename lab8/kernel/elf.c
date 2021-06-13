#include "elf.h"

static uint32_t elf_is_valid(void *p){
    Elf32_Ehdr *h = p;
    if(h->e_ident[EI_MAG0] != 0x7f){
        return 0;
    }
    if(h->e_ident[EI_MAG1] != 'E'){
        return 0;
    }
    if(h->e_ident[EI_MAG2] != 'L'){
        return 0;
    }
    if(h->e_ident[EI_MAG3] != 'F'){
        return 0;
    }
    return 1;
}

int64_t elf_start_offset(void *p){
    if(elf_is_valid(p)){
        Elf32_Ehdr *h = p;
        Elf32_Shdr *sh = p + h->e_shoff;
        return h->e_entry - sh->sh_offset;
    }
    return -1;
}

extern void from_el1_to_el0(void (*main)(), void (*load_t)(), void *sp, void *pgd);
extern void load_template(void (*main)(), uint64_t *sp);

static void *load_setup_argv(char *sp, char *const argv[]){
    if(!argv){
        sp -= 16;
        uint64_t *sp_u64 = (uint64_t *)sp;
        sp_u64[0] = 0;
        sp_u64[1] = 0;
        return sp;
    }

    uint64_t argc = 0;
    char *arg = argv[argc];
    while(arg){
        argc++;
        arg = argv[argc];
    }
    char *argv_temp[argc];
    for(size_t i=1; i<=argc; i++){
        size_t len = strlen(argv[argc - i]) + 1;
        sp -= ROUNDUP_MUL8(len);
        for(size_t j=0; j<len; j++){
            sp[j] = argv[argc - i][j];
        }
        argv_temp[argc - i] = sp;
    }
    sp -= ROUNDUP_MUL16((argc + 3) * 8);
    uint64_t *sp_u64 = (uint64_t *)sp;
    sp_u64[0] = argc;
    sp_u64[1] = (uint64_t)&sp_u64[2];
    for(size_t i=0; i<argc; i++){
        sp_u64[i+2] = (uint64_t)argv_temp[i];
    }
    sp_u64[argc+2] = 0;
    return sp;
}

static int64_t load(const char *find_filename, char *const argv[], Load_return *ret){
    struct file *fp = fopen(find_filename, 0);
    if(!fp){
        return -1;
    }

    size_t size = fp->f_ops->filesize(fp);
    ret->npage = (((size + 0xFFFU) & ~(0xFFFU)) / PAGE_SIZE) + 21;
    char *p = palloc(ret->npage);
    if(p){
        char *tmp = p;
        while(1){
            int n = fread(fp, tmp, ret->npage * PAGE_SIZE);
            if(n == -22){
                break;
            }
            tmp += n;
        }
        ret->user_stack = p;
        ret->sp = load_setup_argv((char *)p + (ret->npage * PAGE_SIZE), argv);
        ret->pc = (void *)p;// + elf_start_offset(p);
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return -1;
}

static void *vm_update(uint64_t *pgd, uint64_t vaddress, uint64_t address){
    uint64_t *page_table = pgd;
    size_t index = (vaddress & ((uint64_t)0b111111111 << 39)) >> 39;
    //printf("1 index = %ld\n", index);
    if(page_table[index] == 0){
        page_table[index] = (uint64_t)palloc(1) | PD_TABLE;
    }

    page_table = (uint64_t *)(page_table[index] & ~0xFFF);
    index = (vaddress & ((uint64_t)0b111111111 << 30)) >> 30;
    //printf("2 index = %ld\n", index);
    if(page_table[index] == 0){
        page_table[index] = (uint64_t)palloc(1) | PD_TABLE;
    }

    page_table = (uint64_t *)(page_table[index] & ~0xFFF);
    index = (vaddress & ((uint64_t)0b111111111 << 21)) >> 21;
    //printf("3 index = %ld\n", index);
    if(page_table[index] == 0){
        page_table[index] = (uint64_t)palloc(1) | PD_TABLE;
    }

    page_table = (uint64_t *)(page_table[index] & ~0xFFF);
    index = (vaddress & ((uint64_t)0b111111111 << 12)) >> 12;
    //printf("4 index = %ld\n", index);
    page_table[index] = address | USER_PTE_ATTR;
}

void *vm_user_init(void *kernel_stack, void *user_stack, size_t user_npage){
    uint64_t *pgd = palloc(1);

    vm_update(pgd, 0x70000, (uint64_t)kernel_stack);

    if(user_npage != 0){
        uint64_t load_template_addr = (uint64_t)load_template & ~0xffff000000000fff;
        vm_update(pgd, load_template_addr, load_template_addr);
        vm_update(pgd, load_template_addr + PAGE_SIZE, load_template_addr + PAGE_SIZE);

        for(size_t i=0; i<user_npage; i++){
            vm_update(pgd, 0x800000 + (i * PAGE_SIZE), (uint64_t)user_stack + (i * PAGE_SIZE));
            //printf("added addr %lx\n", (uint64_t)user_stack + (i * PAGE_SIZE));
        }
    }
    return pgd;
}

int64_t exec(const char* name, char *const argv[]){
    Load_return ret;
    if(load(name, argv, &ret) < 0){
        return -1;
    }

    int64_t pid = fork();
    if(pid < 0){
        free(ret.user_stack);
        return -1;
    }

    if(pid == 0){
        void *phy_load_template = (void *)((uint64_t)load_template & ~0xffff000000000000);
        Thread *child = current_thread();
        child->user_stack_npage = ret.npage;
        child->user_stack = ret.user_stack;
        child->pgd = vm_user_init(child->kernel_stack, child->user_stack, child->user_stack_npage);
        child->va_kernel = child->kernel_stack;
        child->va_user = child->user_stack;
        //from_el1_to_el0(ret.pc, phy_load_template, ret.sp, child->pgd);
        from_el1_to_el0((void *)0x800000, phy_load_template, (void *)(0x800000 + ret.sp - ret.pc), child->pgd);
    }

    return 0;
}

void *fork_vm(void *__parent, void *kernel_stack, void *user_stack, size_t user_npage){
    Thread *parent = __parent;

    uint64_t *pgd = palloc(1);

    vm_update(pgd, (uint64_t)parent->va_kernel, (uint64_t)kernel_stack);

    if(user_npage != 0){
        uint64_t load_template_addr = (uint64_t)load_template & ~0xffff000000000fff;
        vm_update(pgd, load_template_addr, load_template_addr);
        vm_update(pgd, load_template_addr + PAGE_SIZE, load_template_addr + PAGE_SIZE);

        for(size_t i=0; i<user_npage; i++){
            vm_update(pgd, (uint64_t)parent->va_user + (i * PAGE_SIZE), (uint64_t)user_stack + (i * PAGE_SIZE));
            //printf("added addr %lx\n", (uint64_t)user_stack + (i * PAGE_SIZE));
        }
    }
    return pgd;
}
