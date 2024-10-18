#include "circularQueue.h"

// Funzione per inizializzare la coda circolare
circularQueue *initQueue(int size) {
    circularQueue *q = (circularQueue *)calloc(1, sizeof(circularQueue));
    q->size = size + 1;  // Un elemento extra per differenziare pieno/vuoto
    q->arr = (float *)calloc(q->size, sizeof(float));  // Corretto tipo
    q->front = q->rear = 0;
    return q;
}

// Controllo se la coda è vuota
int isEmpty(circularQueue *q) {
    return q->rear == q->front;
}

// Controllo se la coda è piena
int isFull(circularQueue *q) {
    return (q->rear + 1) % q->size == q->front;
}

// Funzione per inserire un elemento nella coda
int enqueue(circularQueue *q, float val) {
    if (isFull(q)) {
        // Sovrascrittura: avanzare il front
        q->front = (q->front + 1) % q->size;
    }
    q->rear = (q->rear + 1) % q->size;
    q->arr[q->rear] = val;  // Inserire il nuovo valore
    return 0;
}

// Funzione per rimuovere un elemento dalla coda
int dequeue(circularQueue *q) {
    if (isEmpty(q)) {
        return -1;  // Coda vuota, nessun elemento da rimuovere
    }
    q->front = (q->front + 1) % q->size;  // Avanza il puntatore front
    return 0;
}

// Funzione per eliminare la coda
void deleteQueue(circularQueue *q) {
    free(q->arr);
    free(q);
}

/*int main() {
    circularQueue *q = initQueue(2);

    // Enqueue di alcuni elementi
    enqueue(q, 12);
    enqueue(q, 15);
    enqueue(q, 17);  // Questo sovrascriverà il primo elemento

    // Dequeue di alcuni elementi
    dequeue(q);
    dequeue(q);

    // Enqueue di nuovi elementi
    enqueue(q, 45);
    enqueue(q, 22);
    enqueue(q, 55);

    if (isEmpty(q)) {
        printf("Queue is empty\n");
    }
    if (isFull(q)) {
        printf("Queue is full\n");
    }

    deleteQueue(q);  // Libera la memoria
    return 0;
}*/