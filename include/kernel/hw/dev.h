#ifndef __DEV_H__
#define __DEV_H__
#include <stdint.h>

typedef struct
{
	uint32_t dev_id;
	char name[20];
	void* addr;
} dev_t;

#endif