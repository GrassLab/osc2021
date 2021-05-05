struct tmpent {
    struct tmpent *rsib;
    struct tmpent *parent;
    struct tmpent *child;
    struct vnode *vnode;
    char name[16];
    int type; // FILEENT for file, DIRENT for directory
    // Following members are only useful for file.
    char *data_start;
    int size;
    int free;
};

#define DIRENT 0
#define FILEENT 1

#define MAX_TMPENT_NR 256


void init_tmpent_pool();
struct tmpent *new_tmpent();
int get_level(const char *pathname);
char *get_level_name(const char *pathname, int level);
int init_tmpfs();
