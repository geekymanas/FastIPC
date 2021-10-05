#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define MAX_RINGBUFS 10
#define RINGBUF_SIZE 16

#define MAP_START MAXVA - 2*PGSIZE - 15*34*PGSIZE 
#define SPACE_JUMP 34*PGSIZE
#define MAXMAPLOC (MAXVA - 36*PGSIZE)

struct spinlock ringbuf_lock;

struct ringbuf {
  int refcount;
  char name[16];
  int pidsRW[2];			//used to tie process id to buffer read/write side
  void* bufPA[RINGBUF_SIZE];		//used to hold all the 16 physical page address of each ring buffer
  void *vabuf;				//used to hold starting virtual address of the each ring buffer
  void *book;				//used to hold physical address of book page
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



//used to cleanup from exit() path when a process exits without calling close on it's ringbufs
//Will iterate over the ringbufs and check the pid and call closeBuf system call on the open ringbufs of that process
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

//A function used to create physical pages and map them to virtual address and also only create virtual mappings if the physical pages are already mapped
int processSpace_mapper(int current_index, char* straddr, int opdesc, int newmap)
{
	struct proc *p = myproc();
	int i;
	void *start = (void*)MAP_START;
	void *x;
	while(walkaddr(p->pagetable, (uint64)start) != 0)
	{
		start = start + SPACE_JUMP;	
		if((unsigned long) start > MAXMAPLOC) return -1;
	}
	x = start;
	ringbufs[current_index].vabuf = start;
	char *mem = 0;
	for(i = 0;i < RINGBUF_SIZE*2;i++, x+=PGSIZE){
		if(i < RINGBUF_SIZE && newmap){
			mem = kalloc();
			ringbufs[current_index].bufPA[i] = (void*)mem;
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


//used to create a ringbuffer and is the client side facing call, which will lookup the ringbufs array to see if it already exists
//and if there is space left in the ringbufs array to assign new ringbuf and calls the process mapper to perform the mappings
int
createbuf(char* straddr, int opdesc, uint64 retvaddr)
{
	if(opdesc < 0 || opdesc > 1) return -1;
	acquire(&ringbuf_lock);
	int i = 0;
	int exists = 1;
	int current_index = -1;
	struct proc *p = myproc();
	for(i = 0;i < MAX_RINGBUFS;i++)
	{
		if(strcmp(straddr, ringbufs[i].name) == 0)
		{
			current_index = i;
			exists = 0;	
			if(ringbufs[i].pidsRW[0] == p->pid || ringbufs[i].pidsRW[1] == p->pid)
			{
				printf("Cannot map same process as reader and writer\n");
				release(&ringbuf_lock);
				return -1;
			}			
			break;
		}
	}
	if(current_index >= 0)
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
		if(current_index == -1){
			release(&ringbuf_lock);
			return -1;
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
	copyout(p->pagetable, retvaddr, (char*)&ringbufs[current_index].vabuf, sizeof(ringbufs[current_index].vabuf));
	acquire(&p->lock);
	p->validRingBuf = 1;
	release(&p->lock);
	release(&ringbuf_lock);
	return exists;
}


//will unmap the virtual and physical mappings depending on the ref count
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
	void* d = ringbufs[current_index].vabuf;
	if(--ringbufs[i].refcount >= 1)
	{
		unmapphypage = 0;
		ringbufs[current_index].pidsRW[opdesc] = 0;
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
        int pidringvalid = 0;	//used to know at exit() path if this process has any valid ringbufs open
        for(i = 0;i < MAX_RINGBUFS;i++)			
        {
        	if(ringbufs[i].pidsRW[0] == p->pid || ringbufs[i].pidsRW[1] == p->pid)
        	{
        		pidringvalid = 1;
        	}
        		
        }
     
        acquire(&p->lock);
    	p->validRingBuf = pidringvalid;
    	release(&p->lock);
	release(&ringbuf_lock);
	return 0;
}
