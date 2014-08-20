#ifndef countdown_latch_h
#define countdown_latch_h

#include <pthread.h>


// Defines a countdown latch that allows one or more threads to wait until a
// set of operations complete. The initial value specifies how many times the
// latch must be counted down before calls to await proceed.
typedef struct countdown_latch_t
{
    int              count;
    pthread_mutex_t* mu;
    pthread_cond_t*  cond;
} countdown_latch_t;

// Initializes a countdown latch and sets its initial value. Sets errno and
// returns NULL if initialization failed.
countdown_latch_t* countdown_latch_init(int count);

// Releases the countdown latch resources.
void countdown_latch_dispose(countdown_latch_t* latch);

// Blocks until the countdown latch reaches 0.
void countdown_latch_await(countdown_latch_t* latch);

// Decrements the countdown latch count, releasing all waiting threads if it
// reaches 0.
void countdown_latch_count_down(countdown_latch_t* latch);

#endif
