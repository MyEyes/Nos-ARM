#ifndef __INT_H__
#define __INT_H__

void set_int_hnd(char interrupt, void* hnd_addr);

void int_init();
#endif