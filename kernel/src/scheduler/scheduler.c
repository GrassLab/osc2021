#include "scheduler.h"
#include "thread.h"
#include "io.h"

struct Thread * runQ[RUNQ_SIZE];
int head = 0, tail = 0;

void do_schedule()
{
    struct Thread *current = get_current_thread();
    if (current->tid != 0) {
        enqueue(current);
    }
    // get next
    struct Thread *next = dequeue();
    set_current_thread( next );
}

// return 0 if success
int enqueue(struct Thread * t)
{
    tail = (tail + 1) % RUNQ_SIZE;
    if (tail == head) {
        tail = (tail - 1) % RUNQ_SIZE;
        return -1;
    } else {
        runQ[tail] = t;
        return 0;
    }
}

struct Thread * dequeue()
{
    if (head == tail) {
        return NULL;
    } else {
        head = (head + 1) % RUNQ_SIZE;
        return runQ[head];
    }
}