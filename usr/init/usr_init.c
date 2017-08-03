#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ipc.h>
#include <sys/types.h>
#include PLATFORM_INCLUDE

volatile char* gpio_dev = 0;
volatile i2c_register* i2c_dev = 0;
volatile char running = 1;
volatile char* dev2;

#define TEST_PIN 23

void __attribute__((naked)) delay(int32_t count)
{
	(void) count;
    __asm__ __volatile__(   "__delay_: subs r0, r0, #1\n"
                            "bne __delay_\n"
                            "bx lr");
}

void thread_test()
{
    while(running)
    { 
            gpio_set(gpio_dev, TEST_PIN);
            delay(0x100000);
            gpio_clr(gpio_dev, TEST_PIN);
            delay(0x100000);
    }
    exit(0);
}

void display_command(uint8_t c)
{
	i2c_start(i2c_dev, 0x3C, 2, 0);
	delay(0x1000);
	i2c_send(i2c_dev, 0);
	i2c_send(i2c_dev, c);
	delay(0x1000);
	puthex(dev2, c);
	puts(" ",dev2);
	puthex(dev2, i2c_dev->status);
    puts("\r\n",dev2);
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
	
    dev2 = req_res("bcm2385_uart0", (void*)0x82000);
    gpio_dev = req_res("bcm2385_gpio", (void*)0x83000);
    i2c_dev = req_res("bcm2385_bsc1", (void*)0x84000);

    if(dev2 == (char*)-1)
    {
        exit(-1);
    }

    i2c_enable(i2c_dev);
	
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

    gpio_func_sel(gpio_dev, 0, 4);
    gpio_func_sel(gpio_dev, 1, 4);
    gpio_func_sel(gpio_dev, 2, 4);
    gpio_func_sel(gpio_dev, 3, 4);
	gpio_set_pullup(gpio_dev, 2, 15);

    puts("\r\nPostDump\r\n", dev2);
    for(int x=0; x<5; x++)
    {
        puthex(dev2, ((uint32_t*)gpio_dev)[x]);
        puts("\r\n", dev2);
    }

    char* new_stack = malloc(0x8000);
    char* thread_start = (char*)((uint32_t)(new_stack+0x8000)&(~0xFFF));
    uint32_t test_thread = thread_create(thread_start, new_stack, thread_test);
    puts("Created thread: ", dev2);
    puthex(dev2, test_thread);
    puts("\r\n",dev2);
    puts("Trying to run thread\r\n", dev2);
    uint32_t run_res = thread_run(test_thread);
    puthex(dev2, run_res);
    puts("\r\n", dev2);

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
        else if(!strcmp(input, "stop"))
        {
            running = !running;
        }
        else if(!strcmp(input, "test"))
        {
            puthex(dev2, i2c_dev->control);
            puts("\r\n",dev2);
            i2c_start(i2c_dev, 0x3C, 0x3, 0);
            delay(0x10000);
            puthex(dev2, i2c_dev->control);
            puts("\r\n",dev2);
            puthex(dev2, i2c_dev->status);
            puts("\r\n",dev2);
            puthex(dev2, i2c_dev->slave_addr);
            puts("\r\n",dev2);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n",dev2);
            puthex(dev2, i2c_send(i2c_dev, 'A'));
            delay(0x10000);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n",dev2);
            puthex(dev2, i2c_send(i2c_dev, 'A'));
            delay(0x10000);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n",dev2);
            puthex(dev2, i2c_send(i2c_dev, '7'));
            delay(0x10000);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n",dev2);
            delay(1000);
            puthex(dev2, i2c_dev->control);
            puts("\r\n", dev2);
            puthex(dev2, i2c_dev->status);
            puts("\r\n", dev2);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n", dev2);
        }
        else if(!strcmp(input, "test2"))
        {
            puthex(dev2, i2c_dev->status);
            puts("\r\n", dev2);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n", dev2);
        }
		else if (!strcmp(input, "test3"))
		{
			for(int x=0; x<127; x++)
			{
				i2c_start(i2c_dev, x, 0x3, 0);
				delay(0x10000);
				i2c_clear_err(i2c_dev);
				delay(0x10000);
				puthex(dev2, x);
				puts(" ", dev2);
				puthex(dev2, i2c_dev->status);
				puts("\r\n", dev2);
			}
		}
		else if (!strcmp(input, "display"))
		{
			//Init
			display_command(0xAE);
			display_command(0xD5);
			display_command(0x80);
			display_command(0xA8);
			display_command(0xFF);
			display_command(0xD3);
			display_command(0x0);
			display_command(0x40 | 0x0);
			display_command(0x8D);
			display_command(0x10);
			display_command(0x20);
			display_command(0x00);
			display_command(0xA0 | 0x1);
			display_command(0xDA);
			display_command(0x12);
			display_command(0x81);
			display_command(0x9F);
			display_command(0xD9);
			display_command(0x22);
			display_command(0xDB);
			display_command(0x40);
			display_command(0xA4);
			display_command(0xA6);
			display_command(0x2E);
			display_command(0xAF);
			
			//Turn on pixels
			//Columns
			display_command(0x21);
			display_command(0x0);
			display_command(0xFF);
			//Pages
			display_command(0x22);
			display_command(0);
			display_command(0x07);
			i2c_start(i2c_dev, 0x3C, 2*128*64/8, 0);
			delay(0x1000);
			for (uint16_t i=0; i<(128*64/8); i++) {
				i2c_send(i2c_dev, 0x40);
				i2c_send(i2c_dev, 0xCC);
			}
			puthex(dev2, i2c_dev->status);
            puts("\r\n", dev2);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n", dev2);
			delay(0x1000);
			puthex(dev2, i2c_dev->status);
            puts("\r\n", dev2);
            puthex(dev2, i2c_dev->data_len);
            puts("\r\n", dev2);
		}
	}
	running = 0;
	exit((int)gpio_dev[GPIO_FSEL_0]);
}
