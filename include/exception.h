#ifndef	_EXCE_H
#define	_EXCE_H

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address);
void sync_exc_router(unsigned long spsr, unsigned long elr, unsigned long esr);
void sync_svc_handler(unsigned long spsr, unsigned long elr, unsigned long esr);
void irq_exc_router();
void print_timestamp();
#endif  /*_EXC_H */

