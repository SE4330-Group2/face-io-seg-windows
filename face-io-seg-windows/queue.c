#include "queue.h"

void queue_init(QUEUE *q)
{
   q->front = 0;
   q->rear = 0;
}

void queue_add(QUEUE *q, int x)
{
   if(!queue_full(q))
   {
      q->items[q->rear] = x;
      q->rear = (q->rear + 1) % MAXQUEUE;
   }
}

int queue_remove(QUEUE *q)
{
   int x = -1;
   if(!queue_empty(q))
   {
      x = q->items[q->front];
      q->front = (q->front + 1) % MAXQUEUE;
   }
   return x;
}

int queue_full(QUEUE *q)
{
   return (q->rear + 1) % MAXQUEUE == q->front;
}

int queue_empty(QUEUE *q)
{
   return q->rear == q->front;
}

