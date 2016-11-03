#ifndef __RES_H__
#define __RES_H__

#include <sys/types.h>

typedef struct{
	res_hndl hndl;
	char name[8];
	void* res;
} res_t;
#endif