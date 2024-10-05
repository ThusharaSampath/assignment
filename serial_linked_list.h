#ifndef SERIAL_LINKED_LIST_H
#define SERIAL_LINKED_LIST_H

#include <stdbool.h>

typedef struct SerialNode {
    int data;
    struct SerialNode* next;
} SerialNode;

typedef struct {
    SerialNode* head;
} SerialLinkedList;

void serial_initialize_list(SerialLinkedList* list);
void serial_cleanup_list(SerialLinkedList* list);
bool serial_member(SerialLinkedList* list, int value);
bool serial_insert(SerialLinkedList* list, int value);
bool serial_delete(SerialLinkedList* list, int value);

#endif // RWL_LINKED_LIST_H