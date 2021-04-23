#include "def.h"
enum STATE {
    RUNNING,
    WAITING,
    FINISHED
};

struct thread {
    int t_id; // ---> thread id
    int p_id; // ---> process id

    uint64_t sp_addr;
    uint64_t pc_addr;
    // state
};