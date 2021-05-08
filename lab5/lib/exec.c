#include "fs.h"
#include "stat.h"
#include "sched.h"
#include "mm.h"
#include "string.h"
#include "fs/cpio.h"

static int exec_binary(struct file *f, const char *argv[]) {
    void *prog = kmalloc(f->size);
    memcpy(prog, f->data, f->size);

    alloc_user_task(prog, argv);

    return 0;
}

int do_exec(const char *path, const char *argv[]) {
    /* TODO: use function array to handle file open/read */
    struct file *f = cpio_get_file(path);
    if (f && (S_IXUSR | f->f_mode)) {
        return exec_binary(f, argv);
    } else {
        return -1;
    }
}