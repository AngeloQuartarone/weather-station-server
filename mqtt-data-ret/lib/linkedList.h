#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef struct node
{
    void *data;
    struct node *next;
} node_t;

typedef struct linked_list
{
    node_t *head;
    pthread_mutex_t lock;
    int size;
} linked_list_t;

void handle_null_error(void *, char *);
void handle_error_en(int, char *);
void message(char *);
void safe_free(void *);
void initialize_list(linked_list_t *);
void add_node(linked_list_t *, void *);
void remove_node(linked_list_t *, void *);
void free_list(linked_list_t *);
void *get_nth_element(linked_list_t *, int );
