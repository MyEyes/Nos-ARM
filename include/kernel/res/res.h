#ifndef __RES_H__
#define __RES_H__

#include <sys/types.h>
#include <stdint.h>
#include <stddef.h>

#define RES_TYPE_UNKWN 0
#define RES_TYPE_PMEM 1
#define RES_TYPE_PORT 2
#define RES_TYPE_DEV 3
#define RES_TYPE_MMDEV 4
#define RES_TYPE_ALL 0xFF

struct res_s{
	uint32_t type;
	size_t size;
	void* addr;
	char* (*getName)(struct res_s*);
    void (*grant)(struct res_s*, v_addr_t);
    void (*remove)(struct res_s*, v_addr_t);
	void (*reset)(struct res_s*);
};

typedef struct res_s res_t;

typedef struct{
	res_id id;
	char* name;
	res_t* res;
} res_hnd;

void res_setDefaultHnd(res_t* res);

void res_create(res_t* res, uint32_t type, size_t size, void* start);

//Default pmem ops
void res_grnt_pmem(res_t* res, v_addr_t addr);
void res_rm_pmem(res_t* res, v_addr_t addr);
char* res_gn_pmem(res_t* res);
void res_rs_pmem(res_t* res);

//Default port ops
void res_grnt_port(res_t* res, v_addr_t addr);
void res_rm_port(res_t* res, v_addr_t addr);
char* res_gn_port(res_t* res);
void res_rs_port(res_t* res);

//Default mmdev ops
void res_grnt_mmdev(res_t* res, v_addr_t addr);
void res_rm_mmdev(res_t* res, v_addr_t addr);
char* res_gn_mmdev(res_t* res);
void res_rs_mmdev(res_t* res);

#endif
