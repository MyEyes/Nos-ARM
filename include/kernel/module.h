#ifndef __MODULE_H__
#define __MODULE_H__

#define MOD_MAGIC 0xdef5

#include <stdint.h>

typedef enum
{
	DRV,
	SRV
} module_type;

typedef void (*mod_init) (void* addr, uint32_t port);

typedef struct
{
	uint32_t magic;
	
	module_type type;
	char mod_name[20];
	char srv_name[20];
	
	uint32_t num_devs;
	char **dev_lst;
	
	uint32_t num_reqs;
	char **req_list;
	
	mod_init init;
} module_hdr_t;

#endif