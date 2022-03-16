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


SEM *sem_init(int initVal) 
{
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
    
    printf("cond: %d\n", &sem->condition_var);
    
    return sem;
}

int sem_del(SEM *sem) 
{

}

void P(SEM *sem) 
{
    pthread_mutex_lock(&sem->mutex);

    printf("1 valuse before p: %d\n", sem->value);

    while (sem->value < 1) {
        pthread_cond_wait(&sem->condition_var, &sem->mutex);
    }

    pthread_cond_signal(&sem->condition_var);
    sem->value--;
    pthread_mutex_unlock(&sem->mutex);
    
    printf("2 value after p: %d\n", sem->value);
}

void V(SEM *sem)
{

}

int main()
{
    SEM *sem = sem_init(4);
    
    P(sem);
    P(sem);
    P(sem);
    P(sem);
    P(sem);

    return 0;
}
