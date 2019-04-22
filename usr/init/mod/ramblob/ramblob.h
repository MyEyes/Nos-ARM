#ifndef __RAMBLOB__H_
#define __RAMBLOB__H_
#include<sys/types.h>

typedef struct {
    char* base_addr;
    char* curr_addr;
    char* end_addr;
    uint32_t size;
} ramblob_t;

int seek(ramblob_t *dev, uint32_t offset);
int read(ramblob_t *dev, uint32_t count, char* target);
int write(ramblob_t *dev, const char* src, uint32_t count);

#endif
