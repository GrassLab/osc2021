#ifndef RESET_H
#define RESET_H

// reboot after watchdog timer expire
void reset(unsigned int tick);
// cancel reset process
void cancel_reset();

#endif