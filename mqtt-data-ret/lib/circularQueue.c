#include "circularQueue.h"

/**
 * Function to initialize the circular queue.
 * 
 * @param size The maximum number of elements the queue can hold (without the extra space for distinguishing full/empty).
 * @return A pointer to the initialized circularQueue structure.
 * 
 * This function allocates memory for the circular queue structure and the underlying array. 
 * It initializes the front and rear indices to 0 and sets the size to `size + 1` to differentiate between full and empty states.
 */
circularQueue *initQueue(int size) {
    circularQueue *q = (circularQueue *)calloc(1, sizeof(circularQueue));
    q->size = size + 1;  // One extra element to differentiate between full/empty
    q->arr = (float *)calloc(q->size, sizeof(float));  // Correct type
    q->front = q->rear = 0;
    return q;
}

/**
 * Function to check if the queue is empty.
 * 
 * @param q The circularQueue pointer to check.
 * @return 1 if the queue is empty, 0 otherwise.
 * 
 * A circular queue is empty when the `front` and `rear` pointers are equal.
 */
int isEmpty(circularQueue *q) {
    return q->rear == q->front;
}

/**
 * Function to check if the queue is full.
 * 
 * @param q The circularQueue pointer to check.
 * @return 1 if the queue is full, 0 otherwise.
 * 
 * The circular queue is full when advancing the `rear` pointer would cause it to overlap the `front` pointer.
 * This is determined by the condition `(rear + 1) % size == front`.
 */
int isFull(circularQueue *q) {
    return (q->rear + 1) % q->size == q->front;
}

/**
 * Function to insert an element into the queue.
 * 
 * @param q The circularQueue pointer.
 * @param val The value to insert (float).
 * @return 0 on success.
 * 
 * If the queue is full, the oldest element is overwritten by advancing the `front` pointer. 
 * The new value is inserted at the `rear` position, and the `rear` pointer is advanced.
 */
int enqueue(circularQueue *q, float val) {
    if (isFull(q)) {
        // Overwrite: advance the front pointer
        q->front = (q->front + 1) % q->size;
    }
    q->rear = (q->rear + 1) % q->size;
    q->arr[q->rear] = val;  // Insert the new value
    return 0;
}

/**
 * Function to remove an element from the queue.
 * 
 * @param q The circularQueue pointer.
 * @return 0 on success, -1 if the queue is empty.
 * 
 * The oldest element is removed by advancing the `front` pointer. 
 * If the queue is empty, the function returns -1.
 */
int dequeue(circularQueue *q) {
    if (isEmpty(q)) {
        return -1;  // Queue is empty, no element to remove
    }
    q->front = (q->front + 1) % q->size;  // Advance the front pointer
    return 0;
}

/**
 * Function to delete the circular queue and free its memory.
 * 
 * @param q The circularQueue pointer to delete.
 * 
 * This function frees the memory allocated for the queue array and the circularQueue structure itself.
 */
void deleteQueue(circularQueue *q) {
    free(q->arr);
    free(q);
}

/* Example main function
int main() {
    circularQueue *q = initQueue(2);

    // Enqueue some elements
    enqueue(q, 12);
    enqueue(q, 15);
    enqueue(q, 17);  // This will overwrite the first element

    // Dequeue some elements
    dequeue(q);
    dequeue(q);

    // Enqueue new elements
    enqueue(q, 45);
    enqueue(q, 22);
    enqueue(q, 55);

    if (isEmpty(q)) {
        printf("Queue is empty\n");
    }
    if (isFull(q)) {
        printf("Queue is full\n");
    }

    deleteQueue(q);  // Free memory
    return 0;
}
*/
