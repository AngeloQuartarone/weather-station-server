#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int size;
    int front;
    int rear;
    float *arr;
} circularQueue;

circularQueue *initQueue(int);
int isEmpty(circularQueue *);
int isFull(circularQueue *);
int enqueue(circularQueue *, float);
int dequeue(circularQueue *);
void deleteQueue(circularQueue *);