#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ipc.h>
#include <sys/types.h>

#define GPIO_SET_1 0x1C
#define GPIO_SET_2 0x20
#define GPIO_CLR_1 0x28
#define GPIO_CLR_2 0x2C
#define GPIO_FSEL_0 0x00

#define GPIO_FUNC_IN 0
#define GPIO_FUNC_OUT 1

#define TEST_PIN 23

#define GPIO_LEVEL_1 0x34
#define GPIO_LEVEL_2 0x38

extern void putc(int c, volatile char* dev);


void puthexbyte(char* dev, char byte)
{
	char high = byte>>4;
	if(high<10)
		putc(0x30+high, dev);
	else
		putc(0x41+(high-10), dev);
	
	char low = byte&0xF;
	if(low<10)
		putc(0x30+low, dev);
	else
		putc(0x41+(low-10), dev);
}


void puthex(char* dev, unsigned int val)
{
	puthexbyte(dev, val>>24);
	puthexbyte(dev, (val>>16)&0xFF);
	puthexbyte(dev, (val>>8)&0xFF);
	puthexbyte(dev, val&0xFF);
}

void gpio_set(uint32_t* dev, char n)
{
    if(n<32)
        dev[GPIO_SET_1>>2] = 1<<n;
    else
        dev[GPIO_SET_2>>2] = 1<<(n-32);
}

void gpio_clr(uint32_t* dev, char n)
{
    if(n<32)
        dev[GPIO_CLR_1>>2] = 1<<n;
    else
        dev[GPIO_CLR_2>>2] = 1<<(n-32);
}

void gpio_func_sel(uint32_t* dev, char n, char val)
{
   char bank = n/10;
   uint32_t curr_val = dev[GPIO_FSEL_0+bank];
   uint32_t offset = n%10;
   uint32_t mask = (7<<(3*offset));
   curr_val &= ~mask;
   curr_val |= (uint32_t)val<<(3*offset);
   dev[GPIO_FSEL_0+bank] = curr_val;
}

char gpio_func_read(uint32_t* dev, char n)
{
   char bank = n/10;
   uint32_t curr_val = dev[GPIO_FSEL_0+bank];
   uint32_t offset = n%10;
   return (curr_val>>(3*offset))&7;
}

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

void __attribute__((naked)) delay(int32_t count)
{
    __asm__ __volatile__(   "__delay_: subs r0, r0, #1\n"
                            "bne __delay_\n"
                            "bx lr");
/*	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : "=r"(count): [count]"0"(count) : "cc");*/
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
    volatile char* gpio_dev = req_res("bcm2385_gpio", (void*)0x83000);

    if(dev2 == (char*)-1)
    {
        exit(-1);
    }
	
	puts("Hellooooo from Usermode\r\n", dev2);
	char c;
    
    ipc_port_t* port = ipc_create_port(1, 0x1000);
    puts("Testing IPC message passing\r\n", dev2);
    const char* test_s = "This is a test message\r\n";
    if(ipc_send_msg(port, 1, strlen(test_s)+1, (char*)test_s))
        exit(-1);
    puts("Sent!\r\n", dev2);
    puts((char*)test_s, dev2);
    if(ipc_read_msg(port, input, 512))
        exit(-1);
    puts("Received\r\n", dev2);
    ipc_msg_t* msg = (ipc_msg_t*) input;
    puts(msg->msg, dev2);

    puts("\r\nPreDump\r\n", dev2);
    for(int x=0; x<5; x++)
    {
        puthex(dev2, ((uint32_t*)gpio_dev)[x]);
        puts("\r\n", dev2);
    }

    puts("\r\n", dev2);

    puthex(dev2, gpio_func_read(gpio_dev, TEST_PIN));
    puts("\r\n", dev2);
    gpio_func_sel(gpio_dev, TEST_PIN, GPIO_FUNC_OUT);
    puthex(dev2, gpio_func_read(gpio_dev, TEST_PIN));
    puts("\r\n", dev2);

    puts("\r\nPostDump\r\n", dev2);
    for(int x=0; x<5; x++)
    {
        puthex(dev2, ((uint32_t*)gpio_dev)[x]);
        puts("\r\n", dev2);
    }

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
        else if(!strcmp(input, "on"))
        {
            gpio_set(gpio_dev, TEST_PIN);
            puts("Turning on GPIO\r\n", dev2);
        }
        else if(!strcmp(input, "off"))
        {
            gpio_clr(gpio_dev, TEST_PIN);
            puts("Turning off GPIO\r\n", dev2);
        }
	}
	
	exit((int)gpio_dev[GPIO_FSEL_0]);
}
