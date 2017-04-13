#ifndef __EHCI_H__
#define __EHCI_H__

//Using http://wiki.osdev.org/Enhanced_Host_Controller_Interface as reference

#include <stdint.h>

typedef struct
{
	uint32_t USB_CMD;			//USB 
	uint32_t USB_STS;			//USB status
	uint32_t USB_INTR; 			//Interrupt enable flag
	uint32_t FRM_IDX;			//Frame index
	uint32_t CTRL_DS_SEGMENT;	//4G Segment Selector
	uint32_t PERIODICLISTBASE;	//Frame List Base
	uint32_t ASYNCLISTADDR;		//Next async list address
	uint32_t dummy[8];
	uint32_t CONFIGFLAG;		//Configured flag register
} ehci_dev_t;

#endif