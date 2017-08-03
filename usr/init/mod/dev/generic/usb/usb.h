#ifndef __USB_H__
#define __USB_H__

//Using http://wiki.osdev.org/Universal_Serial_Bus as a reference
//raspi reference: https://github.com/rsta2/uspi
//Token
#define USB_PID_OUT 	0b0001
#define USB_PID_IN		0b1001
#define USB_PID_SOF		0b0101
#define USB_PID_SETUP	0b1101
//Data
#define USB_PID_DATA0	0b0011
#define USB_PID_DATA1	0b1011
#define USB_PID_DATA2	0b0111
#define USB_PID_MDATA	0b1111
//Handshake
#define USB_PID_ACK		0b0010
#define USB_PID_NAK		0b1010
#define USB_PID_STALL	0b1110
#define USB_PID_NYET	0b0110
//Special
#define USB_PID_PRE		0b1100
#define USB_PID_ERR		0b1100
#define USB_PID_SPLIT	0b1000
#define USB_PID_PING	0b0100

#endif