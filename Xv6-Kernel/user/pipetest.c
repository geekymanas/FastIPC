#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define MSGSIZE 1536
#define MSGSIZE2 128



int main(void)
{

	char* msg1 = "d1jWi0O9lCOnqqFqVJK2uFj8C1NUjeVz1MBl3GpFinL5zCl0Pbr7C3GpMVLOdIyjghrJnxhdxISZyms4qmRzuVZWzoYcGJTHPCILRetnWUQ/UllPErVBteVzXwT32wl84wdSBIRjxkFJdvOuYEgWQyIMNvxwMw+hMCYxWF3vtCquYf043oLbkMoOsaoNbfzysF0W16jgd1jWi0O9lCOnqqFqVJK2uFj8C1NUjeVz1MBl3GpFinL5zCl0Pbr7C3GpMVLOdIyjghrJnxhdxISZyms4qmRzuVZWzoYcGJTHPCILRetnWUQ/UllPErVBteVzXwT32wl84wdSBIRjxkFJdvOuYEgWQyIMNvxwMw+hMCYxWF3vtCquYf043oLbkMoOsaoNbfzysF0W16jgsTZBqVIT594wo8ohV9HzCr2Kg6ZVUnFJJKB-L0Wusvw8Be0tzJn5fBdjVb-q-rJSHuNzaRR9d-vfcA11eIrrL48qs5F6rUDTT_vBOtqs6nhqtPxHadBX5gn3Pt6r-IHFyIU5-QSq4HsB8ZRol2rEnAd1jWi0O9lCOnqqFqVJK2uFj8C1NUjeVz1MBl3GpFinL5zCl0Pbr7C3GpMVLOdIyjghrJnxhdxISZyms4qmRzuVZWzoYcGJTHPCILRetnWUQ/UllPErVBteVzXwT32wl84wdSBIRjxkFJdvOuYEgWQyIMNvxwMw+hMCYxWF3vtCquYf043oLbkMoOsaoNbfzysF0W16jgd1jWi0O9lCOnqqFqVJK2uFj8C1NUjeVz1MBl3GpFinL5zCl0Pbr7C3GpMVLOdIyjghrJnxhdxISZyms4qmRzuVZWzoYcGJTHPCILRetnWUQ/UllPErVBteVzXwT32wl84wdSBIRjxkFJdvOuYEgWQyIMNvxwMw+hMCYxWF3vtCquYf043oLbkMoOsaoNbfzysF0W16jgsTZBqVIT594wo8ohV9HzCr2Kg6ZVUnFJJKB-L0Wusvw8Be0tzJn5fBdjVb-q-rJSHuNzaRR9d-vfcA11eIrrL48qs5F6rUDTT_vBOtqs6nhqtPxHadBX5gn3Pt6r-IHFyIU5-QSq4HsB8ZRol2rEnAd1jWi0O9lCOnqqFqVJK2uFj8C1NUjeVz1MBl3GpFinL5zCl0Pbr7C3GpMVLOdIyjghrJnxhdxISZyms4qmRzuVZWzoYcGJTHPCILRetnWUQ/UllPErVBteVzXwT32wl84wdSBIRjxkFJdvOuYEgWQyIMNvxwMw+hMCYxWF3vtCquYf043oLbkMoOsaoNbfzysF0W16jgd1jWi0O9lCOnqqFqVJK2uFj8C1NUjeVz1MBl3GpFinL5zCl0Pbr7C3GpMVLOdIyjghrJnxhdxISZyms4qmRzuVZWzoYcGJTHPCILRetnWUQ/UllPErVBteVzXwT32wl84wdSBIRjxkFJdvOuYEgWQyIMNvxwMw+hMCYxWF3vtCquYf043oLbkMoOsaoNbfzysF0W16jgsTZBqVIT594wo8ohV9HzCr2Kg6ZVUnFJJKB-L0Wusvw8Be0tzJn5fBdjVb-q-rJSHuNzaRR9d-vfcA11eIrrL48qs5F6rUDTT_vBOtqs6nhqtPxHadBX5gn3Pt6r-IHFyIU5-QSq4HsB8ZRol2rEnA";	
	char *msg2 = "_9OEgUXmnxckXyV-8RLpTMElsaFGq8ZcCvxF4qkx_vjdrN7tYpIsOHntcqdITHlx4iRcZyb_vGfeEELK7Vm4Uw_9OEgUXmnxckXyV-8RLpTMElsaFGq8ZcCvxF4qkx_vjdrN7tYpIsOHntcqdITHlx4iRcZyb_vGfeEELK7Vm4Uw";
	
	int fds[2];
	if(pipe(fds) < 0)
	  exit(0);
	if(fork() == 0){
		int cycles = 6511, i = 0;
		for(i = 0;i < cycles;i++)
		{
			write(fds[1], msg1, MSGSIZE);
		}
		write(fds[1], msg2, MSGSIZE2);
		close(fds[1]);
	}else{
		int j = 0, x = 0, cycles = 6511,i = 0; //cycles for 512*19531 bytes to be sent remaning 128 bytes sent seperately
		int starttime = 0, endtime = 0;
		char buf[MSGSIZE];
		char buf2[MSGSIZE2];
		starttime = uptime();
		for(i = 0;i < cycles;i++)
		{
			x+= read(fds[0], buf, MSGSIZE);
			for(j = 0;j < MSGSIZE;j++)
			{
				if(buf[j]^msg1[j])
				{
					close(fds[0]);
					printf("%c	%c\n",buf[j],msg1[j]);
					wait(&j);
					exit(-1);
				}
			}
		}
		x+= read(fds[0], buf2, MSGSIZE);
		for(j = 0;j < MSGSIZE2;j++)
		{
			if(buf2[j]^msg2[j])
			{
				close(fds[0]);
				printf("%c	%c\n",buf2[j], msg2[j]);
				wait(&j);
				exit(-1);
			}
		}
		close(fds[0]);
		endtime = uptime();
		printf("%d\n",x);
		wait(0);
		printf("Time take : %d\n%d\n%d\n",(endtime-starttime), starttime, endtime);
	}
	exit(0);
	return 0;
}
