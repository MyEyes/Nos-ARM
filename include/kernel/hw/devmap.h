#ifndef __DEVMAP_H__
#define __DEVMAP_H__

#include <kernel/hw/dev.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
	uint32_t 	num_devs;
	size_t		total_size;
	dev_t 		devs[1];
} devmap_t;

extern devmap_t* devmap;

dev_t* devmap_get_dev(char* dev_name);
int parse_devmap();
int set_devmap(devmap_t* devmap);

#endif
