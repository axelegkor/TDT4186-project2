#include <stdio.h>
#include "bbuffer.h"
#include "sem.h"
#include <pthread.h>

typedef struct BNDBUF
{
    int nextin;
    int nextout;
    int bufferSize;
    SEM *empty;
    SEM *full;
    pthread_mutex_t mutex;
    int bufferArray[2048];
} BNDBUF;

BNDBUF *bb_init(unsigned int size)
{
    BNDBUF *bndbuf = malloc(sizeof(struct BNDBUF));

    bndbuf->bufferSize = size;
    bndbuf->nextin = 0;
    bndbuf->nextout = 0;
    bndbuf->empty = sem_init(size);
    bndbuf->full = sem_init(0);
    if (pthread_mutex_init(&bndbuf->mutex, NULL) != 0)
        free(bndbuf);
}

void bb_del(BNDBUF *bb)
{
    if (pthread_mutex_destroy(&bb->mutex) == 0)
    {
        //printf("Mutex destroyed");
    }
    if (sem_del(bb->empty) == 0)
    {
        //prinft("Empty semaphore destroyed");
    }
    if (sem_del(bb->full) == 0)
    {
        //prinft("Full semaphore destroyed");
    }
    free(bb);
}

int bb_get(BNDBUF *bb)
{
    P(bb->full);
    pthread_mutex_lock(&bb->mutex);
    int returnInt = bb->bufferArray[bb->nextout];
    bb->nextout = bb->nextout + 1 % bb->bufferSize;
    pthread_mutex_unlock(&bb->mutex);
    V(bb->empty);
    return returnInt;
}

void bb_add(BNDBUF *bb, int fd)
{
    P(bb->empty);
    pthread_mutex_lock(&bb->mutex);
    bb->bufferArray[bb->nextin] = fd;
    bb->nextin = bb->nextin + 1 % bb->bufferSize;
    pthread_mutex_unlock(&bb->mutex);
    V(bb->full);
}