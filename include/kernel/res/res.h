#ifndef __RES_H__
#define __RES_H__

#include <sys/types.h>

typedef struct{
	uint32_t type;
	size_t size;
	void* start;
	const char* (*getName)();
	const char* (*reset)();
} res_t;

typedef struct{
	res_hndl hndl;
	char name[8];
	res_t* res;
} res_hnd;

#endif