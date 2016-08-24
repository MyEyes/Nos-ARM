#include "kernel/proc/sysmap.h"
#include <stdio.h>

void sysmap_print(sysmap_t* map)
{
	if(map->magic == SYSMAP_MAGIC)
		printf("Sysmap found at %x\r\n", map);
	else
	{
		printf("Sysmap not found at %x\r\nMagic:%x Found:%x\r\n", map, SYSMAP_MAGIC, map->magic);
		return;
	}
	
	char* nameStart = (char*)map + map->names;
	printf("Addr\t\tType\tName\r\n");
	
	for(uint32_t i=0; i<map->num_entries; i++)
		printf("[%x]\t%c\t%s\r\n", map->entries[i].address, (char)map->entries[i].type, nameStart + map->entries[i].name_off);
}