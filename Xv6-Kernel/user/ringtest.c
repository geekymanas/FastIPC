#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/fastipc.h"


#define sz 10000000

int
do_rand(unsigned long *ctx)
{
    long hi, lo, x;

    x = (*ctx % 0x7ffffffe) + 1;
    hi = x / 127773;
    lo = x % 127773;
    x = 16807 * lo - 2836 * hi;
    if (x < 0)
        x += 0x7fffffff;
 
    x--;
    *ctx = x;
    return (x);
}

unsigned long rand_next = 1;

int
rand(void)
{
    return (do_rand(&rand_next));
}

int main(){
	
	// char *writebuffer = (char*) malloc(sizeof(char)*sz);
	// int i =  0, written  = 0;
	// for(i = 0;i < sz;i++){
	// 	writebuffer[i] = rand();
	// }
	// int fd[2];
	// if(pipe(fd) < 0){
	// 	printf("Error\n");
	// 	exit(-1);
	// }

	int i, size, bytes;
	char str1[16] = "Bye";
	unsigned int long counter = 0;

	int status = fork();
	
	if(status < 0)
		exit(-1);
	if(status == 0){		// Write Here
		
		int ringBuf1 = createRingBuf(str1, 1);
		char *write_pointer;

		while (counter < 10)
		{
			size=4096;
			do
			{
				ringbuf_start_write(ringBuf1, &write_pointer, &bytes);
			} while (bytes < size);	
			// printf("Max Write Size: %d\n", bytes);
			i=0;i++;
			for(i=0 ; i<size ; i++){
				*write_pointer = (char) rand();
				write_pointer++; counter++;
			}
			printf("%p\t", write_pointer);
			printf("\n");
			ringbuf_finish_write(ringBuf1, size);
			printf("\n");	
		}
		if (closeRingBuf(str1, ringBuf1, 1) == -1) printf("Error Closing %s\n", str1);
		printf("\nWritten %d bytes\n", counter);

	}else if(status > 0)	// Read Here
	{	
		int ringBuf1 = createRingBuf(str1, 0);
		char *read_pointer;
		char ch;
		while (counter < 10)
		{
			size=4096;
			do
			{
				ringbuf_start_read(ringBuf1, &read_pointer, &bytes);
			} while (bytes < size);	
			// printf("Max Read Size: %d\n", bytes);
			for(i=0 ; i<size ; i++){
				ch = *read_pointer;
				read_pointer++; counter++;
				if (ch != (char) rand()){
					printf("Not Same! %c :: %c\n", ch, (char) i);
					exit(1);
				} else {
					printf("yo ");
				}
			}
			printf("%p\t", read_pointer);
			printf("\n");
			ringbuf_finish_read(ringBuf1, size);
			printf("\n");
		}
		if (closeRingBuf(str1, ringBuf1, 0) == -1) printf("Error Closing %s\n", str1);	
		wait(0);
		printf("\nRead %d bytes\n", counter);
	}
	else{
		exit(-1);
	}
	exit(0);
}

