#ifndef __RINGBUFFER_H_
#define __RINGBUFFER_H_

struct ring_buffer;
struct ring_buffer *alloc_ring_buffer(int size);
int read_buffer(struct ring_buffer *rb, int size, char *outbuf);
int write_buffer(struct ring_buffer *rb, int size, char *inbuf);

#endif