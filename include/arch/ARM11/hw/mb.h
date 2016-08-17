#ifndef __MB_H__
#define __MB_H__

#include <stdint.h>

#define MAIL_BASE 0xB880
#define MAIL_FULL 0x80000000
#define MAIL_EMPTY 0x40000000

#define MAIL_READ_PEEK 0x10
#define MAIL_READ_RW 0x00
#define MAIL_READ_STATUS 0x18
#define MAIL_READ_SENDER 0x14
#define MAIL_READ_CONFIG 0x1C

#define MAIL_WRITE_PEEK 0x30
#define MAIL_WRITE_RW 0x20
#define MAIL_WRITE_STATUS 0x38
#define MAIL_WRITE_SENDER 0x34
#define MAIL_WRITE_CONFIG 0x3C


uint32_t 	mailbox_read(char channel);
void 		mailbox_write(char channel, uint32_t val);

#endif