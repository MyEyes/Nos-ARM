#include "emmc.h"

uint32_t emmc_issue_cmd(bcm2385_emmc_map_t* dev, uint32_t cmd, uint32_t arg1, uint32_t arg2)
{
	dev->arg1 = arg1;
	dev->arg2 = arg2;
	dev->cmdtm = cmd;
	return (uint32_t)-1;
}