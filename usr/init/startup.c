#include <stdlib.h>
extern void main();
void startup()
{
    std_init();
	main();
    exit(0);
}
