#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ipc.h>
#include <sys/types.h>
#include PLATFORM_INCLUDE

#ifdef INIT_UART
volatile char* uart;
void init_puts(char* str){
    puts(str, uart);
}
#else
void init_puts(char* str){

}
#endif

#ifdef BOOTFS
extern char* _binary_bootfs_bootfs_img_start;
extern char* _binary_bootfs_bootfs_img_end;
extern uint32_t _binary_bootfs_bootfs_img_size;
ramblob_t ramblob;
#endif


void main()
{
#ifdef INIT_UART
    uart = req_res("bcm2385_uart0", (void*)0x82000);

    if(uart == (char*)-1)
    {
        exit(-1);
    }
	init_puts("Hellooooo from Usermode\r\n");
#endif
#ifdef BOOTFS
    ramblob.base_addr = _binary_bootfs_bootfs_img_start;
    ramblob.end_addr = _binary_bootfs_bootfs_img_end;
    ramblob.size = _binary_bootfs_bootfs_img_size;
    ramblob.curr_addr = ramblob.base_addr;
    init_puts("Bootfs found\r\n");
#endif	
}
