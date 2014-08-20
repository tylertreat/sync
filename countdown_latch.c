#define _GNU_SOURCE

#ifdef __APPLE__
#define _XOPEN_SOURCE
#endif

#include <errno.h>
#include <stdlib.h>
#include "countdown_latch.h"


// Initializes a countdown latch and sets its initial value. Sets errno and
// returns NULL if initialization failed.
countdown_latch_t* countdown_latch_init(int count)
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

    countdown_latch_t* latch = (countdown_latch_t*) malloc(sizeof(countdown_latch_t));
    if (!latch)
    {
        errno = ENOMEM;
        pthread_mutex_destroy(mu);
        pthread_cond_destroy(cond);
        return NULL;
    }

    latch->count = count;
    latch->mu = mu;
    latch->cond = cond;
    return latch;
}

// Releases the countdown latch resources.
void countdown_latch_dispose(countdown_latch_t* latch)
{
    pthread_mutex_destroy(latch->mu);
    pthread_cond_destroy(latch->cond);
    free(latch);
}

// Blocks until the countdown latch reaches 0.
void countdown_latch_await(countdown_latch_t* latch)
{
    pthread_mutex_lock(latch->mu);
    while (latch->count > 0)
    {
        pthread_cond_wait(latch->cond, latch->mu);
    }
    pthread_mutex_unlock(latch->mu);
}

// Decrements the countdown latch count, releasing all waiting threads if it
// reaches 0.
void countdown_latch_count_down(countdown_latch_t* latch)
{
    pthread_mutex_lock(latch->mu);
    if (--latch->count <= 0)
    {
        pthread_cond_broadcast(latch->cond);
    }
    pthread_mutex_unlock(latch->mu);
}
