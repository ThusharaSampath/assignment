#ifndef RWL_LINKED_LIST_H
#define RWL_LINKED_LIST_H

#include <stdbool.h>
#include <pthread.h>

typedef struct RwlNode {
    int data;
    struct RwlNode* next;
} RwlNode;

typedef struct {
    RwlNode* head;
    pthread_rwlock_t rwlock;
} RwlLinkedList;

void rwl_initialize_list(RwlLinkedList* list);
void rwl_cleanup_list(RwlLinkedList* list);
bool rwl_member(RwlLinkedList* list, int value);
bool rwl_insert(RwlLinkedList* list, int value);
bool rwl_delete(RwlLinkedList* list, int value);

#endif // RWL_LINKED_LIST_H