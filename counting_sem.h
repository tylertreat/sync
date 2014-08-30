#ifndef counting_sem_h
#define counting_sem_h

#include <pthread.h>


// Defines a counting semaphore.
typedef struct counting_sem_t
{
    int             count;
    pthread_mutex_t mu;
    pthread_cond_t  cond;
} counting_sem_t;

// Initializes a semaphore and sets its initial value. Sets errno and returns
// NULL if initialization failed.
counting_sem_t* counting_sem_init(int count);

// Releases the semaphore resources.
void counting_sem_dispose(counting_sem_t* sem);

// Performs a wait operation on the semaphore.
void counting_sem_wait(counting_sem_t* sem);

// Performs a signal operation on the semaphore.
void counting_sem_signal(counting_sem_t* sem);

#endif
