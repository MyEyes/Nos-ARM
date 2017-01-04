#ifndef __EMMC_H__
#define __EMMC_H__
#include <stdint.h>


typedef struct{
	uint32_t	arg2;			//Arg for ACMD23
	uint32_t	blksizecnt;		//31:16 blkcnt, 9:0 blksize
	uint32_t	arg1;			//All commands but ACMD23
	uint32_t	cmdtm;			//29:24 	cmd index
								//23:22 	cmd_type (00 normal, 01 suspend, 10, resume, 11 abort)
								//21 		cmd_is_data 0 no data transfer command
								//20 		check that response has same index as command
								//19 		check the responses crc
								//17:16 	expected response
								//5			multi block
								//4			direction of command (host to card=0) 
								//3:2		command to be sent after completion
								//			00 =none, 01=cmd12, 10=cmd23, 11=res
								//1			block counter enable
	uint32_t	resp0;			
	uint32_t	resp1;
	uint32_t	resp2;
	uint32_t	resp3;
	
	uint32_t	data;
	uint32_t	status;
	
	uint32_t	control0;
	uint32_t	control1;
	
	uint32_t	interrupt;
	uint32_t	irpt_mask;
	uint32_t	irpt_en;
	
	uint32_t	control2;
	
	uint32_t	dummy[4];
	
	uint32_t	force_irpt;
	
	uint32_t	dummy2[7];
	
	uint32_t	boot_timeout;
	
	uint32_t	dbg_sel;
	
	uint32_t	dummy3;
	
	uint32_t	exrdfifo_cfg;
	uint32_t	exrdfifo_en;
	
	uint32_t	tune_step;
	uint32_t	tune_steps_std;
	uint32_t	tune_steps_ddr;
} emmc_map_t;
#endif