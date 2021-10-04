#include "kernel/types.h"
#include "user/user.h"

#define MAX_RINGBUFS 10
#define RINGBUF_SIZE 16
#define BOOKOFFSET 4096*32  // TODO: Check if this is correct

struct user_ring_buf {
  void *buf;
  void *book;
  int exists;
  char name[16];
};

struct user_ring_buf user_ring_bufs[MAX_RINGBUFS];

int
createRingBuf(char* straddr, int opdesc)
{
    unsigned long* ringbuf = (unsigned long*) 0 ;       // TODO: Is this correct?
    if (createbuf(straddr, opdesc, ringbuf) != 0) {
        printf("\nRingbuf could not be created\n");
        return -1;
    }
    if (strlen(straddr) > 16){
        printf("\nSizeof Name is > 16 : %d\n", strlen(straddr));
        return -1;
    }
    
    int i;
    for (i=0 ; i<MAX_RINGBUFS ; i++){
        if (user_ring_bufs[i].exists != 1){
            user_ring_bufs[i].buf = ringbuf;
            user_ring_bufs[i].book = (ringbuf+BOOKOFFSET);
            user_ring_bufs[i].exists = 1;
            strcpy(user_ring_bufs[i].name, straddr);
            return i;
        }
    }
    printf("\nMax Ringbuf Limit Reached\n");
    return -1;
}

int 
closeRingBuf(char* straddr, int rd, int opdesc)
{
    if (user_ring_bufs[rd].exists != 1){
        printf("Invalid Descriptor for Close: %d\n", rd);
        return -1;
    }
    if (strcmp(user_ring_bufs[rd].name, straddr) != 0){
        printf("Invalid Name for Descriptor: %s :: %s\n", straddr, user_ring_bufs[rd].name);
        return -1;
    }
    return closebuf(straddr, opdesc);
}