#include <kernel/hw/devmap.h>
#include <string.h>
#include <kernel/res/res_mgr.h>
#include <stdlib.h>
#include <sys/types.h>

devmap_t* devmap;

dev_t* devmap_get_dev(char* dev_name)
{
	if(!devmap)
		return (dev_t*)0;
	for(uint32_t x=0; x<devmap->num_devs; x++)
	{
		if(!strcmp(dev_name, devmap->devs[x].name))
			return devmap->devs+x;
	}
	return (dev_t*)0;
}

int set_devmap(devmap_t* map)
{
    devmap = map;
    return 0;
}

int parse_devmap()
{
    for(size_t x=0; x<devmap->num_devs; x++)
    {
        res_t* res = malloc(sizeof(res_t));
        uint32_t type = RES_TYPE_UNKWN;
        if(strcmp(devmap->devs[x].type, "mm  "))
            type = RES_TYPE_MMDEV;

        res_create(res, type, 0x1000, (v_addr_t)devmap->devs[x].addr);
        res_register(res, devmap->devs[x].name);
    }
    return 0;
}
