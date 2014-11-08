#include <stdint.h>

#define MAXQUEUE       200

typedef struct {
        uint32_t items[MAXQUEUE];		/* body of queue */
        uint16_t front;               /* position of first element */
        uint16_t rear;                /* position of last element */
} QUEUE;

void queue_init(QUEUE *q);

void queue_add(QUEUE *q, uint32_t x);

uint32_t queue_remove(QUEUE *q);

uint32_t queue_full(QUEUE *q);

uint32_t queue_empty(QUEUE *q);

