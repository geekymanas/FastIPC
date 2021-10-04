#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/fastipc.h"

/* Test Cases to check

< > What if you create more than max number of ringbufs?
< > what happens if you try to close more than the number of ringbufs?

*/

/* Features to implement

< > Checker/Truncater for name
< > Do something about the retval initialization
< > Do we need to initialise refcount and exists to 0 somewhere?

*/


int
main(void)
{
	// unsigned long* retval = (unsigned long*) 1 ;	// Done: Does this work? (unsigned long*) malloc(sizeof(unsigned long*)) ;
	// if(createbuf(str, 0, retval) != 0) return -1;
	// *retval = 10;

	char str1[20] = "0123456789012345";
	char str2[16] = "Bye";
	
	int ringBuf1 = createRingBuf(str1, 0);
	printf("RingBuf Index: %d\n", ringBuf1);
	int ringBuf2 = createRingBuf(str2, 0);
	printf("RingBuf Index: %d\n", ringBuf1);
	printf("RingBuf Index: %d\n", ringBuf2);

	// char **write_pointer = (char **) malloc(sizeof(char **));
	// int *bytes = (int *) malloc(sizeof(int *));
	// ringbuf_start_write(ringBuf1, write_pointer, bytes);
	// printf("Max Write Size: *d", *bytes);
	
	if (closeRingBuf(str1, ringBuf1, 0) == -1) printf("Error Closing %s\n", str1);
	if (closeRingBuf(str2, ringBuf2, 0) == -1) printf("Error Closing %s\n", str2);
	exit(0);
}
