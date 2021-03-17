
#ifndef __UTILS_H__
#define __UTILS_H__

#define uint32_t unsigned long int
#define uint64_t unsigned long long int

struct fdt_header {
    uint32_t address;
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

struct init_driver{
    char name[30];
    void(*callback)(char*, struct fdt_header*, int);
};

#endif