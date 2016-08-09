#ifndef __DMA_H__
#define __DMA_H__

#define DMA_CTRL_REG_N 11			

#define DMA_CTRL_IS_REG 0			//Identity and status
	#define DMA_IS_PRESENT 0
	#define DMA_IS_QUEUED 1
	#define DMA_IS_RUNNING 2
	#define DMA_IS_INTERRUPTING 3
#define DMA_CTRL_UA_REG 1			//User accessibility
#define DMA_CTRL_CHN_REG 2			//Channel number register
#define DMA_CHN_ENAB_REG 3   	//Channel enable reg
	#define DMA_CHN_ENAB_STOP 0
	#define DMA_CHN_ENAB_START 1
	#define DMA_CHN_ENAB_CLEAR 2
#define DMA_CHN_CTRL_REG 4			//DMA control register (for channel)
#define DMA_CHN_INT_START_ADDR 5	//internal start addr
#define DMA_CHN_EXT_START_ADDR 6	//external start addr
#define DMA_CHN_INT_END_ADDR 7		//internal end addr
#define DMA_CHN_STAT_REG	8		//channel status register
#define DMA_CHN_CNTX_ID_REG 15		//context id register

#endif