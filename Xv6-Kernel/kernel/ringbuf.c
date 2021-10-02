#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define PGSIZE 4096

#define MAX_RINGBUFS 10
#define RINGBUF_SIZE 16

#define MAP_START 12288 + (PGSIZE*2) + (128*1024*1024)
#define SPACE_JUMP 34*PGSIZE


#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
#define MAXMAPLOC (MAXVA - 36*PGSIZE)

struct spinlock ringbuf_lock;

struct ringbuf {
  int refcount;
  char name[16];
  int pidsRW[2];
  unsigned long* bufPA[RINGBUF_SIZE];
  unsigned long *vabuf;
  unsigned long *book;
};

struct book {
  unsigned int long read_done, write_done;
};

struct ringbuf ringbufs[MAX_RINGBUFS];

int
closebuf(char* straddr, int opdesc);

int
strcmp(char *p, char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

int ringbufCleanup()
{
	int i = 0;
	struct proc *p = myproc();
	acquire(&ringbuf_lock);
	for(i = 0;i < MAX_RINGBUFS;i++)
	{
		if(ringbufs[i].pidsRW[0] == p->pid)
		{
			release(&ringbuf_lock);
			closebuf(ringbufs[i].name, 0);
			acquire(&ringbuf_lock);
		}
		if(ringbufs[i].pidsRW[1] == p->pid)
		{
			release(&ringbuf_lock);
			closebuf(ringbufs[i].name, 1);
			acquire(&ringbuf_lock);
		}		
			
	}
	release(&ringbuf_lock);
	return 0;
}


int processSpace_mapper(int current_index, char* straddr, int opdesc, int newmap)
{
	struct proc *p = myproc();
	int i;
	unsigned long *start = (unsigned long*)MAP_START;
	unsigned long *x;
	while(walkaddr(p->pagetable, (uint64)start) != 0)
	{
		start = start + SPACE_JUMP;	
		if((unsigned long)start > MAXMAPLOC) return -1;
	}
	x = start;
	ringbufs[current_index].vabuf = start;
	char *mem = 0;
	for(i = 0;i < RINGBUF_SIZE*2;i++, x+=PGSIZE){
		if(i < RINGBUF_SIZE && newmap){
			mem = kalloc();
			ringbufs[current_index].bufPA[i] = (unsigned long*)mem;
		}
		if(mappages(p->pagetable, (uint64)x, PGSIZE, (uint64)ringbufs[current_index].bufPA[i%RINGBUF_SIZE], PTE_U|PTE_R|PTE_W|PTE_V) != 0){
			int point = i-1;
			for(;point>=0;point--)
			{
				uvmunmap(p->pagetable, (uint64)start, 1, 1);
			}
			if(i < RINGBUF_SIZE){
				kfree(mem);
			}
			return -1;
		}
       }
       
       if(newmap){
	       ringbufs[current_index].book = kalloc();
       }
       if(mappages(p->pagetable, (uint64)x, PGSIZE, (uint64)ringbufs[current_index].book, PTE_U|PTE_R|PTE_W|PTE_V) != 0){
       panic("map failed");
				uvmunmap(p->pagetable, (uint64)start, RINGBUF_SIZE*2, 1);
				if(newmap){
					kfree(ringbufs[current_index].book);
				}
				return -1;
				
	}
	return 0;	      
}

int
createbuf(char* straddr, int opdesc, unsigned long* retvaddr)
{
	if(opdesc < 0 || opdesc > 1) return -1;
	acquire(&ringbuf_lock);
	int i = 0;
	int current_index = -1;
	struct proc *p = myproc();
	for(i = 0;i < MAX_RINGBUFS;i++)
	{
		if(strcmp(straddr, ringbufs[i].name) != 0)
		{
			current_index = i;
			break;
		}
	}
	if(current_index > 0)
	{
		if((ringbufs[current_index].pidsRW[opdesc] != 0) || (ringbufs[current_index].refcount < 0)) 
		{	
			release(&ringbuf_lock);
			return -1;
		}
		ringbufs[current_index].pidsRW[opdesc] = p->pid;
		ringbufs[current_index].refcount++;
		if(processSpace_mapper(current_index, straddr, opdesc, 0) != 0) 
		{	
			ringbufs[current_index].pidsRW[opdesc] = 0;
			ringbufs[current_index].refcount--;
			release(&ringbuf_lock);
			return -1;
		}
	
	}
	else
	{
		for(i = 0;i < MAX_RINGBUFS;i++)
		{
			if(ringbufs[i].refcount == 0){
				current_index = i;
				break;
			}
		}
		ringbufs[current_index].refcount++;
		ringbufs[current_index].pidsRW[!opdesc] = 0;		
		ringbufs[current_index].pidsRW[opdesc] = p->pid;
		safestrcpy(ringbufs[current_index].name, straddr, sizeof(straddr));
        	if(processSpace_mapper(current_index, straddr, opdesc, 1) != 0) 
        	{
        		safestrcpy(ringbufs[current_index].name, "", sizeof(0));
			ringbufs[current_index].pidsRW[opdesc] = 0;
        		release(&ringbuf_lock);
        		return -1;
        	}
	}
    	retvaddr = ringbufs[current_index].vabuf;
    	acquire(&p->lock);
    	p->validRingBuf = 1;
    	release(&p->lock);
	release(&ringbuf_lock);
	return 0;
}

int
closebuf(char* straddr, int opdesc)
{
	struct proc *p = myproc();
	int i = 0;
	int current_index = 0;
	int unmapphypage = 1;
	acquire(&ringbuf_lock);
	for(i = 0;i < RINGBUF_SIZE;i++)
	{
		if(strcmp(straddr, ringbufs[i].name) == 0)
		{
			current_index = i;
			break;
		}
	}
	if(current_index == -1) {
		printf("Already Unmapped or not present in RingBuf\n");
		release(&ringbuf_lock);	
		return -1;
	}
	unsigned long* d = ringbufs[current_index].vabuf;
	if(--ringbufs[i].refcount >= 1)
	{
		unmapphypage = 0;
	}
	else
	{
	    ringbufs[current_index].refcount = 0;
     	    *ringbufs[current_index].name = 0;
     	    ringbufs[current_index].pidsRW[0] = 0;
     	    ringbufs[current_index].pidsRW[1] = 0;
     	    ringbufs[current_index].vabuf = 0;
     	    ringbufs[current_index].book = 0;
	}
	
	for(i = 0;i < RINGBUF_SIZE;i++)
	{
	    	uvmunmap(p->pagetable, (uint64) d, 1, unmapphypage);
	    	d+=PGSIZE;
	}	
	for(;i < RINGBUF_SIZE*2;i++)
	{
	    	uvmunmap(p->pagetable, (uint64) d, 1, 0);
	    	d+=PGSIZE;
	}	
        uvmunmap(p->pagetable, (uint64) d, 1, unmapphypage);
        int pidringvalid = 0;
        for(i = 0;i < MAX_RINGBUFS;i++)
        {
        	if(ringbufs[i].pidsRW[0] == p->pid || ringbufs[i].pidsRW[1] == p->pid)
        	{
        		pidringvalid = 1;
        		printf("Here\n");
        	}
        		
        }
     
        acquire(&p->lock);
    	p->validRingBuf = pidringvalid;
    	release(&p->lock);
	release(&ringbuf_lock);
	return 0;
}
