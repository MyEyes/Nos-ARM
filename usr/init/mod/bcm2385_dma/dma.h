#ifndef __DMA_H__
#define __DMA_H__

#define DMA_INT 0xfe0
#define DMA_ENABLE 0xff0

#include <stdint.h>

typedef struct{
	uint32_t	tr_inf;
	uint32_t	src_addr;
	uint32_t	dst_addr;
	uint32_t	tr_len;
	uint32_t	stride;
	void*		next_blk;
	uint32_t	res[2];
} dma_ctrl_blk_t;


typedef struct{
	uint32_t			cs;
	dma_ctrl_blk_t*		con_blk;
	uint32_t			ti;
	uint32_t			src;
	uint32_t			dst;
	uint32_t			len;
	uint32_t			stride;
	uint32_t			next_blk;
	uint32_t			dbg;
} dma_chn_regs_t;

#endif