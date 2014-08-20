#define _GNU_SOURCE

#ifdef __APPLE__
#define _XOPEN_SOURCE
#endif

#include <errno.h>
#include <stdlib.h>

#include "counting_sem.h"


// Initializes a semaphore and sets its initial value. Sets errno and returns
// NULL if initialization failed.
counting_sem_t* counting_sem_init(int count)
{
    pthread_mutex_t* mu = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    if (!mu)
    {
        errno = ENOMEM;
        return NULL;
    }

    if (pthread_mutex_init(mu, NULL) != 0)
    {
        free(mu);
    }

    pthread_cond_t* cond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    if (!cond)
    {
        errno = ENOMEM;
        pthread_mutex_destroy(mu);
    }

    if (pthread_cond_init(cond, NULL) != 0)
    {
        free(cond);
        pthread_mutex_destroy(mu);
        return NULL;
    }

    counting_sem_t* sem = (counting_sem_t*) malloc(sizeof(counting_sem_t));
    if (!sem)
    {
        errno = ENOMEM;
        pthread_mutex_destroy(mu);
        pthread_cond_destroy(cond);
        return NULL;
    }

    sem->count = count;
    sem->mu = mu;
    sem->cond = cond;
    return sem;
}

// Releases the semaphore resources.
void counting_sem_dispose(counting_sem_t* sem)
{
    pthread_mutex_destroy(sem->mu);
    pthread_cond_destroy(sem->cond);
    free(sem);
}

// Performs a wait operation on the semaphore.
void counting_sem_wait(counting_sem_t* sem)
{
    pthread_mutex_lock(sem->mu);
    while (sem->count <= 0)
    {
        pthread_cond_wait(sem->cond, sem->mu);
    }
    sem->count--;
    pthread_mutex_unlock(sem->mu);
}

// Performs a signal operation on the semaphore.
void counting_sem_signal(counting_sem_t* sem)
{
    pthread_mutex_lock(sem->mu);
    int prevCount = sem->count++;
    pthread_mutex_unlock(sem->mu);
    if (prevCount == 0)
    {
        pthread_cond_signal(sem->cond);
    }
}

