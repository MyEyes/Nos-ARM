#ifndef __DEV_H__
#define __DEV_H__
#include <stdint.h>

#define DEV_NAME_MAX_LENGTH 20
#define DEV_TYPE_MAX_LENGTH 4

typedef struct
{
	uint32_t dev_id;
	char name[DEV_NAME_MAX_LENGTH];
	char type[DEV_TYPE_MAX_LENGTH];
	uint32_t addr;
} dev_t;

#endif