#include <stdio.h>
#include "bbuffer.h"


typedef struct BNDBUF
{
    int head;
    int tail;
    int size;
    int bufferArray[size];
} BNDBUF;

BNDBUF *bb_init(unsigned int size)
{
    
}