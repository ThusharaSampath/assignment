#include "mtx_linked_list.h"
#include <stdlib.h>
#include <stdio.h>

void mtx_initialize_list(MtxLinkedList *list)
{
    list->head = NULL;
    pthread_mutex_init(&list->mutex, NULL);
}

void mtx_cleanup_list(MtxLinkedList *list)
{
    pthread_mutex_lock(&list->mutex);
    MtxNode *current = list->head;
    while (current != NULL)
    {
        MtxNode *temp = current;
        current = current->next;
        free(temp);
    }
    pthread_mutex_unlock(&list->mutex);
    pthread_mutex_destroy(&list->mutex);
}

bool mtx_member(MtxLinkedList *list, int value)
{
    pthread_mutex_lock(&list->mutex);
    MtxNode *current = list->head;
    while (current != NULL)
    {
        if (current->data == value)
        {
            pthread_mutex_unlock(&list->mutex);
            return true;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&list->mutex);
    return false;
}

bool mtx_insert(MtxLinkedList *list, int value)
{
    pthread_mutex_lock(&list->mutex);
    // if (mtx_member(list, value, false))
    // {
    //     pthread_mutex_unlock(&list->mutex);
    //     return false;
    // }
    MtxNode *new_node = malloc(sizeof(MtxNode));
    if (new_node == NULL)
    {
        printf("Failed to allocate memory for new node\n");
        pthread_mutex_unlock(&list->mutex);
        return false;
    }
    new_node->data = value;
    new_node->next = list->head;
    list->head = new_node;
    pthread_mutex_unlock(&list->mutex);
    return true;
}

bool mtx_delete(MtxLinkedList *list, int value)
{
    pthread_mutex_lock(&list->mutex);
    if (list->head == NULL)
    {
        pthread_mutex_unlock(&list->mutex);
        return false;
    }
    if (list->head->data == value)
    {
        MtxNode *temp = list->head;
        list->head = list->head->next;
        free(temp);
        pthread_mutex_unlock(&list->mutex);
        return true;
    }
    MtxNode *current = list->head;
    while (current->next != NULL)
    {
        if (current->next->data == value)
        {
            MtxNode *temp = current->next;
            current->next = temp->next;
            free(temp);
            pthread_mutex_unlock(&list->mutex);
            return true;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&list->mutex);
    return false;
}