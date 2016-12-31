#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ipc.h>
#include <sys/types.h>

void putc(int c, volatile char* dev)
{
	//Wait until device ready
	//This only works for the bcm2385_uart0
	//So there's clearly some work left to be done
	while((dev[0x18] & (1 << 5))){ }
	*dev = c;
}

void puts(char* s, volatile char* dev)
{
	while(*s)
		putc(*s++, dev);
}

unsigned char getc(volatile char* dev)
{
    // Wait for UART to have recieved something.
    while ( dev[0x18] & (1 << 4) ) { }
    return dev[0];
}

void main(uint32_t tid)
{
	(void) tid;
	std_init();
	int* test = malloc(sizeof(int));
	
	test[0]=tid;
	
	free(test);
	
	int* test_2 = malloc(sizeof(int));
	*test_2 = 32;

    char *input = malloc(512);
	
    volatile char* dev2 = req_res("bcm2385_uart0", (void*)0x82000);
	
    if(dev2 == (char*)-1)
    {
        exit(-1);
    }
	
	puts("Hellooooo from Usermode\r\n", dev2);
	char c;
    /*
    ipc_port_t* port = ipc_create_port(1, 0x1000);
    puts("Testing IPC message passing\r\n", dev2);

    const char* test_s = "This is a test message\r\n";
    if(ipc_send_msg(port, 1, strlen(test_s), (char*)test_s))
        exit(-1);
    puts("Sent!\r\n", dev2);
    puts((char*)test_s, dev2);
    if(ipc_read_msg(port, input, 512))
        exit(-1);
    puts("Received\r\n", dev2);
    ipc_msg_t* msg = (ipc_msg_t*) input;
    puts(msg->msg, dev2);
    */


	unsigned short position = 0;
	char quit = 0;
	while(!quit)
	{
		while((c = getc(dev2))!='\r' && c!='\n')
		{
			putc(c, dev2);
			input[position++] = c;
		}
		input[position++] = 0;
		puts("\r\n", dev2);
		position = 0;
		if(!strcmp(input, "quit"))
			quit = 1;
	}
	
	exit((int)dev2);
}
