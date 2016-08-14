#ifndef __SYSMAP_H__
#define __SYSMAP_H__

#define SYSMAP_MAGIC 0xF00F00AA

#include <stdint.h>

typedef enum
{
	SM_ABSOLUTE 		= 'A',
	SM_BSS 			= 'B',
	SM_COMMON 			= 'C',
	SM_DATA			= 'D',
	SM_SDATA			= 'G',
	SM_INDIRECT		= 'i',
	SM_DEBUG			= 'N',
	SM_STACK_UNWND		= 'p',
	SM_READ_ONLY		= 'R',
	SM_SBSS			= 'S',
	SM_TEXT			= 'T',
	SM_UNDEFINED		= 'U',
	SM_UNIQUE			= 'u',
	SM_WEAK			= 'V',
	SM_WEAK2			= 'W',
	SM_STABS			= '-',
	SM_UNKNOWN			= '?'
	
} sysmap_entry_type_t;

#define SYS_ENTRY_ISGLOBAL(u) (u<97)
#define SYS_ENTRY_NORM_TYPE(u) (SYS_ENTRY_ISGLOBAL(u)?u:u-32)

typedef struct
{
	uint32_t			address;
	sysmap_entry_type_t type;
	uint32_t			name_off;
} sysmap_entry_t;

typedef struct
{
	uint32_t		magic;
	uint32_t 		total_size;
	uint32_t		num_entries;
	uint32_t		names;
	sysmap_entry_t	entries[1];
} sysmap_t;

void sysmap_print(sysmap_t* addr);

#endif