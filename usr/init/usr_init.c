#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

void main(uint32_t tid)
{
	(void) tid;
	std_init();
	int* test = malloc(sizeof(int));
	
	test[0]=tid;
	
	free(test);
	
	int* test_2 = malloc(sizeof(int));
	*test_2 = 32;
	
	exit((int)test_2);
}