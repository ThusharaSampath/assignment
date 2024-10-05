#include "rwl_linked_list.h"
#include <stdlib.h>
#include <stdio.h>

void rwl_initialize_list(RwlLinkedList *list)
{
    list->head = NULL;
    pthread_rwlock_init(&list->rwlock, NULL);
}

void rwl_cleanup_list(RwlLinkedList *list)
{
    pthread_rwlock_wrlock(&list->rwlock);
    RwlNode *current = list->head;
    while (current != NULL)
    {
        RwlNode *temp = current;
        current = current->next;
        free(temp);
    }
    pthread_rwlock_unlock(&list->rwlock);
    pthread_rwlock_destroy(&list->rwlock);
}

bool rwl_member(RwlLinkedList *list, int value)
{
    pthread_rwlock_rdlock(&list->rwlock);
    RwlNode *current = list->head;
    while (current != NULL)
    {
        if (current->data == value)
        {
            pthread_rwlock_unlock(&list->rwlock);
            return true;
        }
        current = current->next;
    }
    pthread_rwlock_unlock(&list->rwlock);
    return false;
}

bool rwl_insert(RwlLinkedList *list, int value)
{
    pthread_rwlock_wrlock(&list->rwlock);
    // if (rwl_member(list, value, false))
    // {
    //     pthread_rwlock_unlock(&list->rwlock);
    //     return false;
    // }
    RwlNode *new_node = malloc(sizeof(RwlNode));
    if (new_node == NULL)
    {
        printf("Failed to allocate memory for new node\n");
        pthread_rwlock_unlock(&list->rwlock);
        return false;
    }
    new_node->data = value;
    new_node->next = list->head;
    list->head = new_node;
    pthread_rwlock_unlock(&list->rwlock);
    return true;
}

bool rwl_delete(RwlLinkedList *list, int value)
{
    pthread_rwlock_wrlock(&list->rwlock);
    if (list->head == NULL)
    {
        pthread_rwlock_unlock(&list->rwlock);
        return false;
    }
    if (list->head->data == value)
    {
        RwlNode *temp = list->head;
        list->head = list->head->next;
        free(temp);
        pthread_rwlock_unlock(&list->rwlock);
        return true;
    }
    RwlNode *current = list->head;
    while (current->next != NULL)
    {
        if (current->next->data == value)
        {
            RwlNode *temp = current->next;
            current->next = temp->next;
            free(temp);
            pthread_rwlock_unlock(&list->rwlock);
            return true;
        }
        current = current->next;
    }
    pthread_rwlock_unlock(&list->rwlock);
    return false;
}