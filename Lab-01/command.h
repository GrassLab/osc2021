#ifndef COMMAND_H
#define COMMAND_H

void input_buffer_overflow_message ( char [] );

void command_help ();
void command_hello ();
void command_not_found ( char * );
void reset();
void cancel_reset();
#endif