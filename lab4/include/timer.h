# include "mem_addr.h"

# define PRINT_TIMER_DEFAULT_FREQ  2000  // unit : ms
# define MAX_TIMER_QUEUE_SIZE 10

struct timer_object{
  unsigned long long target_cval;
  unsigned long long reg_cval;
  int token;
  void (*func)(int);
  timer_object *pre;
  timer_object *next;
};

struct one_shot_timer{
  unsigned int ms;
  int token;
  void (*func)(int);
};

void print_timer(unsigned long long ms, char *comment);
extern "C"
void core_timer_init();
void timer_queue_insert(unsigned long long tval, int token, void (*func)(int));
void set_cval_register();
inline void core_timer_enable();
inline void core_timer_disable();
void print_system_time_enable();
void print_system_time_disable();
void core_timer_interrupt_handler();
void get_core_timer_value(unsigned long long* r);
void get_core_timer_ms(unsigned long long* r);
void print_system_timer_cb(int token);
void set_next_pst_tval();
void set_one_shot_timer(struct one_shot_timer*);
