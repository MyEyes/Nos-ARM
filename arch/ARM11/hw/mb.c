#include <arch/ARM11/hw/mb.h>
#include __PLATFORM__

void mailbox_write(char channel, uint32_t val)
{
	void* addr = (void*)(PERIPHERALS_LOW_ADDR + MAIL_BASE);
	while(*((uint32_t*)(addr+MAIL_WRITE_STATUS)) & MAIL_FULL)
	{
		//wait
	}
	
	*((uint32_t*)(addr+MAIL_WRITE_RW)) = (val<<4) + channel;
}