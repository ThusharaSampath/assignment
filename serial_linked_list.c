#include "serial_linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void serial_initialize_list(SerialLinkedList *list)
{
    list->head = NULL;
}

void serial_cleanup_list(SerialLinkedList *list)
{
    SerialNode *current = list->head;
    while (current != NULL)
    {
        SerialNode *temp = current;
        current = current->next;
        free(temp);
    }
}

bool serial_member(SerialLinkedList *list, int value)
{
    SerialNode *current = list->head;
    while (current != NULL)
    {
        if (current->data == value)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool serial_insert(SerialLinkedList *list, int value)
{
    SerialNode *new_node = malloc(sizeof(SerialNode));
    if (new_node == NULL)
    {
        printf("Failed to allocate memory for new node\n");
        return false;
    }
    new_node->data = value;
    new_node->next = list->head;
    list->head = new_node;
    return true;
}

bool serial_delete(SerialLinkedList *list, int value)
{
    if (list->head == NULL)
    {
        return false;
    }
    if (list->head->data == value)
    {
        SerialNode *temp = list->head;
        list->head = list->head->next;
        free(temp);
        return true;
    }
    SerialNode *current = list->head;
    while (current->next != NULL)
    {
        if (current->next->data == value)
        {
            SerialNode *temp = current->next;
            current->next = temp->next;
            free(temp);
            return true;
        }
        current = current->next;
    }
    return false;
}