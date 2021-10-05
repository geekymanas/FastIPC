#include "kernel/types.h"
#include "user/user.h"

#define MAX_RINGBUFS 10
#define RINGBUF_SIZE 16
#define BUF_SIZE 65536
#define BOOKOFFSET 131072  // TODO: Check if this is correct

#define print

typedef struct book {
  unsigned int long read_done, write_done;
} __attribute__((__may_alias__)) book ;

struct user_ring_buf {
  void *buf;
  book *book;    // TODO: Is this okay?
  int exists;
  char name[16];
};

void store(long unsigned int *p, int v) {
  __atomic_store_8(p, v, __ATOMIC_SEQ_CST);
}

int load(long unsigned int *p) {
  return __atomic_load_8(p, __ATOMIC_SEQ_CST);
}

struct user_ring_buf user_ring_bufs[MAX_RINGBUFS];

int
createRingBuf(char* straddr, int opdesc)
{
    void* ringbuf;       // TODO: Is this correct?
    int exists = createbuf(straddr, opdesc, &ringbuf);
    if (exists == -1) {
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
            if (exists){
                store(&user_ring_bufs[i].book->read_done, 0);
                store(&user_ring_bufs[i].book->write_done, 0);
                // printf("RingBuf Created with :\t");
            }
            else
                // printf("RingBuf already exists with :\t");

            user_ring_bufs[i].exists = 1;
            strcpy(user_ring_bufs[i].name, straddr);
            // printf("Loc: %p,%p\tName: %s\t", user_ring_bufs[i].buf, user_ring_bufs[i].book, user_ring_bufs[i].name);
            // printf("Book Page: %d,%d\n", user_ring_bufs[i].book->read_done, user_ring_bufs[i].book->write_done);
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
    user_ring_bufs[rd].exists = 0;
    return closebuf(straddr, opdesc);
}

int 
ringbuf_start_write(int rd, char **addr, int *bytes)
{
    if ((rd > MAX_RINGBUFS) || (user_ring_bufs[rd].exists != 1)){
        printf("Invalid Descriptor for Read Start: %d\n", rd);
        return -1;
    }

    unsigned int long read = load(&user_ring_bufs[rd].book->read_done);
    unsigned int long write = load(&user_ring_bufs[rd].book->write_done);
    if ((BUF_SIZE - (write - read)) < 0){
        printf("Start Write: Buffer is -ve\n");
        exit(1);
    }

    *bytes = BUF_SIZE - (write - read);                // TODO: How to Handle Full Buffers?
    *addr = (char *) user_ring_bufs[rd].buf;
    // printf("Start Write: %d, %d, %p, %p\n", read, write, *addr, user_ring_bufs[rd].buf);
    return 0;
}

int
ringbuf_finish_write(int rd, int bytes)         // TODO: Need any checks here?
{
    if ((rd > MAX_RINGBUFS) || (user_ring_bufs[rd].exists != 1)){
        printf("Invalid Descriptor for Read Start: %d\n", rd);
        return -1;
    }
    // unsigned int long read = load(&user_ring_bufs[rd].book->read_done);
    unsigned int long write = load(&user_ring_bufs[rd].book->write_done);
    // printf("Finish Write1: %d, %d\n", read, write);
    store(&user_ring_bufs[rd].book->write_done, write+bytes);

    // write = load(&user_ring_bufs[rd].book->write_done);
    
    // printf("Finish Write2: %d, %d\n", read, write);
    // printf("Written %d bytes (Remaining: %d)\n", bytes, (write - read));
    return 0;
}

int
ringbuf_start_read(int rd, char **addr, int *bytes)
{
    if ((rd > MAX_RINGBUFS) || (user_ring_bufs[rd].exists != 1)){
        printf("Invalid Descriptor for Read Start: %d\n", rd);
        return -1;
    }

    unsigned int long read = load(&user_ring_bufs[rd].book->read_done);
    unsigned int long write = load(&user_ring_bufs[rd].book->write_done);
    if ((BUF_SIZE - (write - read)) < 0){
        printf("Start Read: Buffer is -ve\n");
        exit(1);
    }

    *bytes = (write - read);
    *addr = (char *) user_ring_bufs[rd].buf;
    // printf("Start Read: %d, %d, %d, %p, %p\n", read, write, *bytes, *addr, user_ring_bufs[rd].buf);
    return 0;
}

int
ringbuf_finish_read(int rd, int bytes)         // TODO: Need any checks here?
{
    if ((rd > MAX_RINGBUFS) || (user_ring_bufs[rd].exists != 1)){
        printf("Invalid Descriptor for Read Start: %d\n", rd);
        return -1;
    }
    unsigned int long read = load(&user_ring_bufs[rd].book->read_done);
    // unsigned int long write = load(&user_ring_bufs[rd].book->write_done);
    // printf("Finish Read1: %d, %d\n", read, write);
    store(&user_ring_bufs[rd].book->read_done, read+bytes);

    // read = load(&user_ring_bufs[rd].book->read_done);
    
    // printf("Finish Read2: %d, %d\n", read, write);
    // printf("Read %d:%d:%d bytes (Remaining: %d)\n", write, read, BUF_SIZE, (write - read)%BUF_SIZE);
    return 0;
}
