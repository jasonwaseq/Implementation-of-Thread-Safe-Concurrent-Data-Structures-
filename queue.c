// Jason Waseq
// cse130
// asgn3
// queue.c

#include "queue.h"
#include <stdlib.h>
#include <pthread.h>

struct queue {
    void **buffer;
    int capacity;
    int front;
    int rear;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
};

queue_t *queue_new(int size) {
    if (size <= 0) {
        return NULL;
    }
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL;
    }
    q->buffer = malloc(size * sizeof(void *));
    if (q->buffer == NULL) {
        free(q);
        return NULL;
    }
    q->capacity = size;
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    return q;
}

void queue_delete(queue_t **q) {
    if (q == NULL || *q == NULL) {
        return;
    }
    queue_t *tmp = *q;
    pthread_mutex_destroy(&tmp->lock);
    pthread_cond_destroy(&tmp->not_full);
    pthread_cond_destroy(&tmp->not_empty);
    free(tmp->buffer);
    free(tmp);
    *q = NULL;
}

bool queue_push(queue_t *q, void *elem) {
    if (q == NULL) {
        return false;
    }
    pthread_mutex_lock(&q->lock);
    while (q->count == q->capacity) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    q->buffer[q->rear] = elem;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
    return true;
}

bool queue_pop(queue_t *q, void **elem) {
    if (q == NULL) {
        return false;
    }
    pthread_mutex_lock(&q->lock);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    *elem = q->buffer[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return true;
}
