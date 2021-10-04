#include "kernel/types.h"
#include "user/user.h"

#define MAX_RINGBUFS 10
#define RINGBUF_SIZE 16
#define BUF_SIZE RINGBUF_SIZE*4096
#define BOOKOFFSET 4096*0  // TODO: Check if this is correct

typedef struct book {
  unsigned int long read_done, write_done;
} __attribute__((__may_alias__)) book ;

struct user_ring_buf {
  void *buf;
  book *book;    // TODO: Is this okay?
  int exists;
  char name[16];
};

void store(int *p, int v) {
  __atomic_store_8(p, v, __ATOMIC_SEQ_CST);
}

int load(int *p) {
  return __atomic_load_8(p, __ATOMIC_SEQ_CST);
}

struct user_ring_buf user_ring_bufs[MAX_RINGBUFS];

int
createRingBuf(char* straddr, int opdesc)
{
    void* ringbuf = (void*) 0 ;       // TODO: Is this correct?
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
            user_ring_bufs[i].book = (book *) (ringbuf+BOOKOFFSET);
            user_ring_bufs[i].book->read_done = 0;
            // user_ring_bufs[i].book->write_done = 0;
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
    if ((rd > MAX_RINGBUFS) || (user_ring_bufs[rd].exists != 1)){
        printf("Invalid Descriptor for Close: %d\n", rd);
        return -1;
    }
    if (strcmp(user_ring_bufs[rd].name, straddr) != 0){
        printf("Invalid Name for Descriptor: %s :: %s\n", straddr, user_ring_bufs[rd].name);
        return -1;
    }
    return closebuf(straddr, opdesc);
}

int 
ringbuf_start_write(int rd, char **addr, int *bytes)
{
    if ((rd > MAX_RINGBUFS) || (user_ring_bufs[rd].exists != 1)){
        printf("Invalid Descriptor for Read Start: %d\n", rd);
        return -1;
    }

    // unsigned int long read = *load(*(user_ring_bufs[rd].book->read_done));       TODO: Make Atomic
    // unsigned int long write = *load(*(user_ring_bufs[rd].book->write_done));
    unsigned int long read = user_ring_bufs[rd].book->read_done;
    unsigned int long write = user_ring_bufs[rd].book->write_done;
    // *bytes = BUF_SIZE;
    *bytes = BUF_SIZE - (write - read)%BUF_SIZE;
    *addr = (char *) user_ring_bufs[rd].buf;
    return 0;
}