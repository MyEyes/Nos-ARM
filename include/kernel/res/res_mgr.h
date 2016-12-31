#ifndef __RES_MGR_H__
#define __RES_MGR_H__

#include <kernel/res/res.h>

#define RES_MGR_INIT_SIZE 128

void res_mgr_init();
res_id res_register(res_t* res, char* name);
res_hnd* res_req(res_id hndl);
res_hnd* res_req_nm(char* name);

res_hnd* res_tbl_get_free();
int res_tbl_grw(size_t new_sz);
void res_tbl_dbg();
#endif
