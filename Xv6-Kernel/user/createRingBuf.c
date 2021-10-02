#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
	unsigned long* retval = (unsigned long*) malloc(sizeof(unsigned long*)) ;
	char str[16] = "Hello";
	if(createbuf(str, 0, retval) != 0) return -1;
	*retval = 10;
	closebuf(str, 0);
	exit(0);
}
