#ifndef __MODULE_H__
#define __MODULE_H__

#define MOD_MAGIC 0xdef5

#include <stdint.h>

typedef enum
{
	DRV,
	SRV
} module_type;

typedef (*mod_init) (void* addr, uint32_t port);

typedef struct
{
	uint32_t magic;
	
	module_type type;
	char name[20];
	
	uint32_t num_devs;
	char **dev_lst;
	
	mod_init init;
} module_hdr_t;



#endif