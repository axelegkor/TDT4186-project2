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
    SEM *sem = malloc(sizeof(struct SEM));  // Struct SEM??

    if (sem == NULL)
        return NULL;

    sem->value = initVal;;

    if (pthread_mutex_init(&sem->mutex, NULL) != 0)
        free(sem);


    if (pthread_cond_init(&sem->condition_var, NULL) != 0)
        return pthread_mutex_destroy(&sem->mutex);
    
    return sem;
}

int sem_del(SEM *sem) 
{    
    if (pthread_mutex_destroy(&sem->mutex) == 0) {
        if (pthread_cond_destroy(&sem->condition_var) == 0){
            free(sem);
            sem->value = 0;

            printf("Sem has been destoryed\n");
            printf("value: %d\n", sem->value);
            return 0;
        }
        return -1;
    } 
    return -1;
}

void P(SEM *sem) 
{
    pthread_mutex_lock(&sem->mutex);

    while (sem->value < 1) {
        pthread_cond_wait(&sem->condition_var, &sem->mutex);
    }

    pthread_cond_signal(&sem->condition_var);
    sem->value--;
    pthread_mutex_unlock(&sem->mutex);
}

void V(SEM *sem)
{
    pthread_mutex_lock(&sem->mutex);
    pthread_cond_signal(&sem->condition_var);
    sem->value++;
    pthread_mutex_unlock(&sem->mutex);
}

