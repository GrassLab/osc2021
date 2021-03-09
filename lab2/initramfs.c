
struct cpio_newc_header { // 110 bytes
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};

// | header | pathname | data |
// |  110   |         4x     4x

int check_cpio_header(char *ptr)
{
    struct cpio_newc_header *ent_ptr;

    ent_ptr = (struct cpio_newc_header*) ptr;
    if (strcmp_with_len(ent_ptr->c_magic, "070701", 6)) {
        uart_send_string("Error: c_magic wrong");
        return 1;
    }
    return 0;
}

int get_cpio_namesize(char *ptr)
{
    struct cpio_newc_header *ent_ptr;

    ent_ptr = (struct cpio_newc_header*) ptr;
    ent_ptr->c_namesize
}