#ifndef MTX_LINKED_LIST_H
#define MTX_LINKED_LIST_H

#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct MtxNode {
    int data;
    struct MtxNode* next;
} MtxNode;

typedef struct {
    MtxNode* head;
    pthread_mutex_t mutex;
} MtxLinkedList;

void mtx_initialize_list(MtxLinkedList* list);
void mtx_cleanup_list(MtxLinkedList* list);
bool mtx_member(MtxLinkedList* list, int value);
bool mtx_insert(MtxLinkedList* list, int value);
bool mtx_delete(MtxLinkedList* list, int value);

#endif // MTX_LINKED_LIST_H