#include "kernel/proc/schd.h"
#include "kernel/proc/thread.h"
#include <stdint.h>

thread_t* threads[20];

void schd_init()
{
	for(uint32_t x=0; x<20; x++)
	{
		threads[x] = 0;
	}
}

void schd_chg_thread()
{
	for(uint32_t x=0; x<20; x++)
	{
		if(threads[x] && threads[x]!=curr_thread)
		{
			thread_change(threads[x]);
			break;
		}
	}
}

void schd_add_thread(thread_t* thread)
{
	for(uint32_t x=0; x<20; x++)
	{
		if(!threads[x])
		{
			threads[x] = thread;
			return;
		}
	}
}