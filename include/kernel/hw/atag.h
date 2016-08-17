#ifndef __ATAG_H__
#define __ATAG_H__
#include <stdint.h>

#define ATAG_NONE 	0x00000000
#define ATAG_CORE 	0x54410001
#define ATAG_MEM 	0x54410002
#define ATAG_VIDEOTEXT 	0x54410003
#define ATAG_RAMDISK 	0x54410004
#define ATAG_INITRD2 	0x54410005
#define ATAG_SERIAL 	0x54410006
#define ATAG_REVISION 	0x54410007
#define ATAG_VIDEOLFB 	0x54410008
#define ATAG_CMDLINE 	0x54410009

typedef struct{
	uint32_t flags;
	uint32_t pagesize;
	uint32_t rootdev;
} atag_core_t;

typedef struct
{
	uint32_t size;
	uint32_t start;
} atag_mem_t;

typedef struct
{
	char 	x;
	char 	y;
	unsigned short 	video_page;
	char	video_mode;
	char	video_cols;
	char	video_ega_bx;
	char	video_lines;
	char	video_isvga;
	char	video_points;
} atag_videotext_t;

typedef struct
{
	uint32_t flags;
	uint32_t size;
	uint32_t start;
} atag_ramdisk_t;

typedef struct
{
	uint32_t start;
	uint32_t size;
} atag_initrd2_t;

typedef struct
{
	uint32_t low;
	uint32_t high;
} atag_serial_t;

typedef struct
{
	uint32_t rev;
} atag_revision_t;

typedef struct
{
	char cmdline[1];
} atag_cmdline_t;

typedef struct
{
	uint32_t size;
	uint32_t tag;
	union {
		atag_core_t 		core;
		atag_mem_t 			mem;
		atag_videotext_t 	video;
		atag_ramdisk_t		ramdisk;
		atag_initrd2_t		initrd2;
		atag_serial_t		serial;
		atag_revision_t		revision;
		atag_cmdline_t		cmdline;
	} val;
} atag_t;

#endif
