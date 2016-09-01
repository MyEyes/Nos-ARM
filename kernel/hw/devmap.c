#include <kernel/hw/devmap.h>
#include <string.h>

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