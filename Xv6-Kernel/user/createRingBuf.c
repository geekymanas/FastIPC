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

	// char str[16] = "hello";
	// char* retval;
	// if(createbuf(str, 0, &retval) != 0) return -1;
	// printf("%p\n",retval);

	// int i = 0;
	// printf("\n");
	// for(i = 0; i < 33; i++)
	// {
	// 	printf(">> %d\n", i);
	// 	*(retval+4096*i) = i;	
	// }
	// for(i = 0; i < 33; i++)
	// {
	// 	printf(">> %d : ", i);
	// 	printf("%d\n",*(retval+4096*i));	
	// }	 
	// printf("%p\n",retval);
	 
    char str1[20] = "0123456789012345";
	//char str2[16] = "Bye";

	
	int ringBuf1 = createRingBuf(str1, 0);
	printf("RingBuf Index: %d\n", ringBuf1);
	int ringBuf2 = createRingBuf(str1, 1);
	//printf("RingBuf Index: %d\n", ringBuf1);
	printf("RingBuf Index: %d\n", ringBuf2);
	//printf("\n");printf("\n");

	/*char *write_pointer, *read_pointer;
	int bytes, i, size;
	char ch;
	
	// Write 1
	size=4096;
	do
	{
		ringbuf_start_write(ringBuf1, &write_pointer, &bytes);
	} while (bytes < size);	
	printf("Max Write Size: %d\n", bytes);
	i=0;i++;
	for(i=0 ; i<size ; i++){
		*write_pointer = (char) i;
		write_pointer++;
	}
	printf("%p\t", write_pointer);
	printf("\n");
	ringbuf_finish_write(ringBuf1, size);
	printf("\n");

	// Write 2
	size=4096;
	do
	{
		ringbuf_start_write(ringBuf1, &write_pointer, &bytes);
	} while (bytes < size);	
	printf("Max Write Size: %d\n", bytes);
	i=0;i++;
	for(i=0 ; i<size ; i++){
		*write_pointer = (char) i;
		write_pointer++;
	}
	printf("%p\t", write_pointer);
	printf("\n");
	ringbuf_finish_write(ringBuf1, size);
	printf("\n");

	// Read 1
	size=4096;
	do
	{
		ringbuf_start_read(ringBuf1, &read_pointer, &bytes);
	} while (bytes < size);	
	printf("Max Read Size: %d\n", bytes);
	for(i=0 ; i<size ; i++){
		ch = *read_pointer;
		read_pointer++;
		if (ch != (char) i){
			printf("Not Same! %c :: %c\n", ch, (char) i);
			exit(1);
		}
	}
	printf("%p\t", read_pointer);
	printf("\n");
	ringbuf_finish_read(ringBuf1, size);
	printf("\n");

	// Read 2
	size=4096;
	do
	{
		ringbuf_start_read(ringBuf1, &read_pointer, &bytes);
	} while (bytes < size);	
	printf("Max Read Size: %d\n", bytes);
	for(i=0 ; i<size ; i++){
		ch = *read_pointer;
		read_pointer++;
		if (ch != (char) i){
			printf("Not Same! %c :: %c\n", ch, (char) i);
			exit(1);
		}
	}
	printf("%p\t", read_pointer);
	printf("\n");
	ringbuf_finish_read(ringBuf1, size);
	printf("\n");

	// Read 3
	size=4096;
	do
	{
		ringbuf_start_read(ringBuf1, &read_pointer, &bytes);
	} while (bytes < size);	
	printf("Max Read Size: %d\n", bytes);
	for(i=0 ; i<size ; i++){
		ch = *read_pointer;
		read_pointer++;
		if (ch != (char) i){
			printf("Not Same! %c :: %c\n", ch, (char) i);
			exit(1);
		}
	}
	printf("%p\t", read_pointer);
	printf("\n");
	ringbuf_finish_read(ringBuf1, size);
	printf("\n");*/

	
	if (closeRingBuf(str1, ringBuf1, 0) == -1) printf("Error Closing %s\n", str1);
	if (closeRingBuf(str1, ringBuf2, 1) == -1) printf("Error Closing %s\n", str1);
	exit(0);
}
