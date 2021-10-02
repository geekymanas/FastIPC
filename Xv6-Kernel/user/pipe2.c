#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

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
	
	char *writebuffer = (char*) malloc(sizeof(char)*sz);
	int i =  0, written  = 0;
	for(i = 0;i < sz;i++){
		writebuffer[i] = rand();
	}
	int fd[2];
	if(pipe(fd) < 0){
		printf("Error\n");
		exit(-1);
	}
	int status = fork();
	if(status < 0)
		exit(-1);
	if(status == 0){
		close(fd[0]);
		while(written != sz){
			int tmp = write(fd[1], (writebuffer+written), (sz-written));
			if(tmp < 0) exit(-1);
			written+= tmp;
		}
//		printf("Total Written : %d\n", written);

	}else{
		if(status > 0)
		{
			char *readbuffer = (char*) malloc(sizeof(char)*sz);
			int readol = 0;
			int starttime = 0, endtime = 0;
			close(fd[1]);
			starttime = uptime();
			while(readol != sz){
				int tmp = read(fd[0], readbuffer+readol, (sz-readol));	
				if(tmp < 0)
					exit(-1);
				readol+= tmp;
			}
			endtime = uptime();
			for(i = 0;i < sz;i++){
				if(writebuffer[i] != readbuffer[i]){
					printf("%d %d\n",writebuffer[i], readbuffer[i]);
					exit(-1);
				}
			}
			
			wait(0);
			//printf("Total Read : %d\n", readol);
			printf("Total Time Taken : %d\n",(endtime-starttime));

		}
		else{
			exit(-1);
		}
		
	}
	exit(0);
}
