#include <stdio.h>
#include "sem.h"
#include <pthread.h>
#include <stdlib.h>


typedef struct SEM 
{
    pthread_mutex_t mutex;
    pthread_cond_t condition_var;
    int value; // votile int??
} SEM; 


SEM *sem_init(int initVal) {
    SEM *sem = malloc(sizeof(SEM));  // Struct SEM??

    if (sem == NULL)
        return NULL;

    sem->value = initVal;
    printf("value: %d\n", initVal);
    printf("sem: %d\n", sem);

    if (pthread_mutex_init(&sem->mutex, NULL) != 0)
        free(sem);
    
    printf("mutex: %d\n", &sem->mutex);

    if (pthread_cond_init(&sem->condition_var, NULL) != 0)
        return pthread_mutex_destroy(&sem->mutex);
    
    printf("cond: %d", &sem->condition_var);
    
    return sem;
}



int main()
{
    sem_init(4);

    return 0;
}
