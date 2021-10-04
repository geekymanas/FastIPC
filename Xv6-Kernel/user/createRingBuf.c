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

	 char str[16] = "hello";
	 char* retval;
	 if(createbuf(str, 0, &retval) != 0) return -1;
	 printf("%p\n",retval);
	 
	 int i = 0;
	 printf("\n");
	 for(i = 0; i < 33; i++)
	 {
	 	printf(">> %d : ", i);
	 	*(retval+4096*i) = i;	
	 }
	 for(i = 0; i < 33; i++)
	 {
	 	printf(">> %d : ", i);
	 	printf("%d\n",*(retval+4096*i));	
	 }
	 
	 
//	 printf("%p\n",retval);



//	 int i = 0;
	 
        // char str1[20] = "0123456789012345";
	 //char str2[16] = "Bye";






	
	/*int ringBuf1 = createRingBuf(str1, 0);
	printf("RingBuf Index: %d\n", ringBuf1);
	int ringBuf2 = createRingBuf(str2, 0);
	printf("RingBuf Index: %d\n", ringBuf1);
	printf("RingBuf Index: %d\n", ringBuf2);
*/
	// char **write_pointer = (char **) malloc(sizeof(char **));
	// int *bytes = (int *) malloc(sizeof(int *));
	// ringbuf_start_write(ringBuf1, write_pointer, bytes);
	// printf("Max Write Size: *d", *bytes);
	
	//if (closeRingBuf(str1, ringBuf1, 0) == -1) printf("Error Closing %s\n", str1);
	//if (closeRingBuf(str2, ringBuf2, 0) == -1) printf("Error Closing %s\n", str2);
	exit(0);
}
