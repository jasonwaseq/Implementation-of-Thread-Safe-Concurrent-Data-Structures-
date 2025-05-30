// Jason Waseq
// cse130
// asgn3
// rwlock.c

#include "rwlock.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

struct rwlock {
    pthread_mutex_t lock;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;

    uint32_t active_readers;
    uint32_t waiting_readers;
    uint32_t active_writers;
    uint32_t waiting_writers;

    PRIORITY priority;
    uint32_t n_value;
    uint32_t readers_since_writer;
};

rwlock_t *rwlock_new(PRIORITY p, uint32_t n) {
    rwlock_t *rw = (rwlock_t *) malloc(sizeof(rwlock_t));
    if (rw == NULL) {
        return NULL;
    }

    if (pthread_mutex_init(&rw->lock, NULL) != 0) {
        free(rw);
        return NULL;
    }

    if (pthread_cond_init(&rw->readers_cond, NULL) != 0) {
        pthread_mutex_destroy(&rw->lock);
        free(rw);
        return NULL;
    }

    if (pthread_cond_init(&rw->writers_cond, NULL) != 0) {
        pthread_cond_destroy(&rw->readers_cond);
        pthread_mutex_destroy(&rw->lock);
        free(rw);
        return NULL;
    }

    rw->active_readers = 0;
    rw->waiting_readers = 0;
    rw->active_writers = 0;
    rw->waiting_writers = 0;
    rw->priority = p;
    rw->n_value = (p == N_WAY) ? n : 0;
    rw->readers_since_writer = 0;

    return rw;
}

void rwlock_delete(rwlock_t **rw) {
    if (rw == NULL || *rw == NULL) {
        return;
    }

    rwlock_t *lock = *rw;

    pthread_cond_destroy(&lock->writers_cond);
    pthread_cond_destroy(&lock->readers_cond);
    pthread_mutex_destroy(&lock->lock);

    free(lock);

    *rw = NULL;
}

static bool can_reader_acquire(rwlock_t *rw) {
    if (rw->active_writers > 0) {
        return false;
    }

    switch (rw->priority) {
    case READERS: return true;
    case WRITERS: return (rw->waiting_writers == 0);
    case N_WAY:
        if (rw->waiting_writers == 0) {
            return true;
        }
        return (rw->readers_since_writer < rw->n_value);
    default: return false;
    }
}

static bool can_writer_acquire(rwlock_t *rw) {
    if (rw->active_readers > 0 || rw->active_writers > 0) {
        return false;
    }

    switch (rw->priority) {
    case WRITERS: return true;
    case READERS: return (rw->waiting_readers == 0);
    case N_WAY:
        if (rw->waiting_readers == 0) {
            return true;
        }
        return (rw->readers_since_writer >= rw->n_value);
    default: return false;
    }
}

void reader_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    rw->waiting_readers++;

    while (!can_reader_acquire(rw)) {
        pthread_cond_wait(&rw->readers_cond, &rw->lock);
    }

    rw->waiting_readers--;
    rw->active_readers++;
    rw->readers_since_writer++;

    pthread_mutex_unlock(&rw->lock);
}

void reader_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    rw->active_readers--;

    if (rw->active_readers == 0) {
        pthread_cond_signal(&rw->writers_cond);
    }

    pthread_mutex_unlock(&rw->lock);
}

void writer_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    rw->waiting_writers++;

    while (!can_writer_acquire(rw)) {
        pthread_cond_wait(&rw->writers_cond, &rw->lock);
    }

    rw->waiting_writers--;
    rw->active_writers = 1;
    rw->readers_since_writer = 0;

    pthread_mutex_unlock(&rw->lock);
}

void writer_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);

    rw->active_writers = 0;

    pthread_cond_broadcast(&rw->readers_cond);
    pthread_cond_signal(&rw->writers_cond);

    pthread_mutex_unlock(&rw->lock);
}
