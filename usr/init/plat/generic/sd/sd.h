#ifndef __SD_H__
#define __SD_H__



//Using http://www.chlazza.net/sdcardinfo.html as reference

#define SD_CMD_GO_IDLE 					0
#define SD_CMD_SEND_OP_COND 			1
#define SD_CMD_SWITCH_FUNC 				6
#define SD_CMD_SEND_IF_COND 			8
#define SD_CMD_SEND_CSD 				9   //Card specific data
#define SD_CMD_SEND_CID 				10	//Card identification
#define SD_CMD_STOP_TRANSMISSION 		12			
#define SD_CMD_SEND_STATUS 				13
#define SD_CMD_SET_BLK_LEN 				16
#define SD_CMD_READ_SING_BLOCK 			17
#define SD_CMD_READ_MULT_BLOCK 			18
#define SD_CMD_WRITE_SING_BLOCK 		24
#define SD_CMD_WRITE_MULT_BLOCK 		25
#define SD_CMD_PROGRAM_CSD 				27
#define SD_CMD_SET_WRITE_PROT 			28
#define SD_CMD_CLR_WRITE_PROT			29
#define SD_CMD_SEND_WRITE_PROT			30
#define SD_CMD_ERASE_WR_BLK_START		32
#define SD_CMD_ERASE_WE_BLK_END			33
#define SD_CMD_ERASE					38
#define SD_CMD_LOCK_UNLOCK				42
#define SD_CMD_APP_CMD					55
#define SD_CMD_GEN_CMD					56
#define SD_CMD_READ_OCR					58
#define SD_CMD_CRC_ON_OFF				59
#endif