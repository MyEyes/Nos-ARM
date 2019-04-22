#include "ramblob.h"
#include <string.h>

int rb_seek(ramblob_t *dev, uint32_t offset){
    if(offset >= dev->size){
        return -1;
    }
    dev->curr_addr = dev->base_addr+offset;
    return 0;
}

int rb_read(ramblob_t *dev, uint32_t count, char* target){
    //Make sure we don't read past the end of our ramblob
    if(count > (uint32_t)(dev->end_addr-dev->curr_addr)){
        count = (uint32_t)(dev->end_addr-dev->curr_addr);
    }
    memcpy(target, dev->curr_addr, count);
    dev->curr_addr += count;
    return count;
}

int rb_write(ramblob_t *dev, const char* src, uint32_t count){
    if(count > (uint32_t)(dev->end_addr-dev->curr_addr)){
        count = (uint32_t)(dev->end_addr-dev->curr_addr);
    }
    memcpy(dev->curr_addr, src, count);
    return count;
}
