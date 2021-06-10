#include <stddef.h>
#include <sysreg.h>
#include <mm.h>
#include <mmu.h>
#include <string.h>
#include <elf.h>
#include <printf.h>
#include <current.h>

#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) |  (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE 0
#define MAIR_IDX_NORMAL 1

#define IDENTITY_TT_L0 ((pd_t *)0x0)
#define IDENTITY_TT_L1 ((pd_t *)0x1000)
#define IDENTITY_TT_L0_VA ((pd_t *)phys_to_virt(0x0))
#define IDENTITY_TT_L1_VA ((pd_t *)phys_to_virt(0x1000))
#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_PAGE 0b11
#define PD_ACCESS (1 << 10)
#define PD_NS (1 << 5)
#define PD_USER_RW (0b01 << 6)
#define PD_USER_R  (0b11 << 6)
#define PD_UXN (1L << 54)
#define PD_PXN (1L << 53)
#define MMU_ENABLE 1

size_t get_PTE(size_t va) {
    size_t flags = disable_irq_save();

    asm("at s1e0r, %0\n\t" : : "r"(va));
    size_t pa = read_sysreg(par_el1);

    irq_restore(flags);

    return pa;
}

void switch_mm() {
    asm(
        "dsb ish\n\t"           /* ensure write has completed */
        "msr ttbr0_el1, %0\n\t" /* switch ttb */
        "tlbi vmalle1is\n\t"    /* invalidate all TLB entries */
        "dsb ish\n\t"           /* ensure completion of TLB invalidatation */
        "isb\n\t"               /* clear pipeline */
    ::"r"(current->ttbr0));
}

void map_user_page(pd_t *tbl, unsigned long va, unsigned long pa, int pflags) {
    for (int lvl = 0; lvl < 4; lvl++) {
        unsigned idx = (va >> (39 - 9 * lvl)) & 0b111111111;

        if (!tbl[idx]) {
            if (lvl == 3) {
                unsigned long flags;

                if (pflags & PF_W) {
                    flags = PD_USER_RW;
                } else if (pflags & PF_R) {
                    flags = PD_USER_R;
                }

                if (!(pflags & PF_X)) {
                    flags |= PD_UXN;
                }
                tbl[idx] = pa;
                tbl[idx] |= PD_PXN | PD_ACCESS | (MAIR_IDX_NORMAL << 2) | PD_NS | PD_PAGE | flags;
                return;
            } else {
                tbl[idx] = (pd_t)virt_to_phys(kcalloc(PAGE_SIZE));
                tbl[idx] |= PD_ACCESS | (MAIR_IDX_NORMAL << 2) | PD_NS | PD_TABLE;
            }
        }

        if (tbl[idx] & 0b11 == PD_TABLE) {
            tbl = (pd_t *)phys_to_virt(tbl[idx] & ENTRY_PA_MASK);

        } else {
            panic("el0 page table corrupted");
        }
    }
}

static void _free_user_vm(pd_t *tbl, int lvl) {
    for (int i = 0; i < 512; i++) {
        if (tbl[i]) {
            if (i < 3) {
                pd_t *nxt = (pd_t *)phys_to_virt(tbl[i] & ENTRY_PA_MASK);
                _free_user_vm(nxt, lvl + 1);
            } else {
                kfree(phys_to_virt(tbl[i] & ENTRY_PA_MASK));
            }
        }
    }
    kfree(tbl);
}

void free_user_vm(pd_t *tbl) {
    _free_user_vm(tbl, 0);
}

void free_vm_area(struct vm_area *vm) {
    struct vm_area *tmp;

    while (vm) {
        tmp = vm;
        vm = vm->next;
        kfree(tmp);
    }
}

void *alloc_user_stack(pd_t *tbl, unsigned long size) {
    void *stack = kcalloc(size);
    for (int i = 0; i < size; i++) {
        map_user_page(tbl, USTACK_VA + i, virt_to_phys((char *)stack + i), PF_R | PF_W);
    }

    return stack;
}

void setup_identity_mapping() {
    /* setup tcr_el1 for 4KB grandle size */
    write_sysreg(tcr_el1, TCR_CONFIG_DEFAULT);

    /* setup two attr entry in mair_el1 for PTE to reference */
    size_t mair_attr_01 =
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE * 8)) |
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL * 8));
    write_sysreg(mair_el1, mair_attr_01);

    /* clean up space */
    memset(IDENTITY_TT_L0, 0, 0x1000);
    memset(IDENTITY_TT_L1, 0, 0x1000);

    /* setup identity mapping (0 - 7fffffff) in ttbr0_el1 */
    IDENTITY_TT_L0[0] = (pd_t)IDENTITY_TT_L1 | PD_TABLE;
    IDENTITY_TT_L1[0] = 0x00000000 | PD_ACCESS | (MAIR_IDX_DEVICE << 2) | PD_NS | PD_BLOCK;
    IDENTITY_TT_L1[1] = 0x40000000 | PD_ACCESS | (MAIR_IDX_DEVICE << 2) | PD_NS | PD_BLOCK;

    /* enable mmu on EL0/1 */
    write_sysreg(ttbr0_el1, IDENTITY_TT_L0);
    write_sysreg(ttbr1_el1, IDENTITY_TT_L0);
    unsigned long sctlr = read_sysreg(sctlr_el1);
    write_sysreg(sctlr_el1, sctlr | MMU_ENABLE);
}

void setup_kernel_space_mapping() {
    /* 2MB block mapping */
    pd_t *p1 = kcalloc(PAGE_SIZE);
    for (int i = 0; i < 256; i++) {
        p1[i] = (i << 21) | PD_ACCESS | (MAIR_IDX_DEVICE << 2) | PD_NS | PD_BLOCK;
    }
    for (int i = 256; i < 512; i++) {
        p1[i] = (i << 21) | PD_ACCESS | (MAIR_IDX_NORMAL << 2) | PD_NS | PD_BLOCK;
    }

    pd_t *p2 = kcalloc(PAGE_SIZE);
    for (int i = 0; i < 512; i++) {
        p2[i] = 0x40000000 | (i << 21) | PD_ACCESS | (MAIR_IDX_DEVICE << 2) | PD_NS | PD_BLOCK;
    }

    asm("dsb ish\n\t");         /* ensure write has completed */

    IDENTITY_TT_L1_VA[0] = (pd_t)virt_to_phys(p1) | PD_TABLE;
    IDENTITY_TT_L1_VA[1] = (pd_t)virt_to_phys(p2) | PD_TABLE;

    asm(
        "tlbi vmalle1is\n\t"    /* invalidate all TLB entries */
        "dsb ish\n\t"           /* ensure completion of TLB invalidatation */
        "isb\n\t"               /* clear pipeline */
    );
}