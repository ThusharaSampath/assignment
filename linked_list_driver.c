#include "mtx_linked_list.h"
#include "rwl_linked_list.h"
#include "serial_linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX_VALUE 65535 // 2^16 - 1

typedef struct
{
    void *list;
    int num_operations;
    double member_fraction;
    double insert_fraction;
    double delete_fraction;
    int type;
} ThreadArgs;

bool is_in_array(int *arr, int size, int value)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == value)
        {
            return true;
        }
    }
    return false;
}

void generate_unique_random_numbers(int *arr, int n)
{
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        int num;
        do
        {
            num = rand() % (MAX_VALUE + 1);
        } while (is_in_array(arr, i, num));
        arr[i] = num;
    }
}

void populate_mtx_list(MtxLinkedList *list, int *numbers, int n)
{
    for (int i = 0; i < n; i++)
    {
        mtx_insert(list, numbers[i]);
    }
}

void populate_rwl_list(RwlLinkedList *list, int *numbers, int n)
{
    for (int i = 0; i < n; i++)
    {
        rwl_insert(list, numbers[i]);
    }
}

void populate_serial_list(SerialLinkedList *list, int *numbers, int n)
{
    for (int i = 0; i < n; i++)
    {
        serial_insert(list, numbers[i]);
    }
}

void *perform_operations(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    for (int i = 0; i < args->num_operations; i++)
    {
        double op = (double)rand() / RAND_MAX;
        int value = rand() % (MAX_VALUE + 1);

        switch (args->type)
        {
        case 1: // read write lock
        {
            RwlLinkedList *casted_list = (RwlLinkedList *)args->list;
            if (op < args->member_fraction)
            {
                rwl_member(casted_list, value);
            }
            else if (op < args->member_fraction + args->insert_fraction)
            {
                rwl_insert(casted_list, value);
            }
            else
            {
                rwl_delete(casted_list, value);
            }
            break;
        }
        case 2:
        {
            MtxLinkedList *casted_list = (MtxLinkedList *)args->list;
            if (op < args->member_fraction)
            {
                mtx_member(casted_list, value);
            }
            else if (op < args->member_fraction + args->insert_fraction)
            {
                mtx_insert(casted_list, value);
            }
            else
            {
                mtx_delete(casted_list, value);
            }
            break;
        }
        default:
        {
            SerialLinkedList *casted_list = (SerialLinkedList *)args->list;
            if (op < args->member_fraction)
            {
                serial_member(casted_list, value);
            }
            else if (op < args->member_fraction + args->insert_fraction)
            {
                serial_insert(casted_list, value);
            }
            else
            {
                serial_delete(casted_list, value);
            }
            break;
        }
        }
    }
    return NULL;
}

void run_test(int type, int n, int m, double mMember, double mInsert, double mDelete, int t, int *randNumbers)
{
    void *list;
    switch (type)
    {
    case 1: // read write lock
    {
        RwlLinkedList *rwl_list = malloc(sizeof(RwlLinkedList));
        printf("Initializing rwl list\n");
        rwl_initialize_list(rwl_list);
        printf("Populating rwl list\n");
        populate_rwl_list(rwl_list, randNumbers, n);
        list = rwl_list;
        break;
    }
    case 2: // mutex
    {
        MtxLinkedList *mtx_list = malloc(sizeof(MtxLinkedList));
        printf("Initializing mtx list\n");
        mtx_initialize_list(mtx_list);
        printf("Populating mtx list\n");
        populate_mtx_list(mtx_list, randNumbers, n);
        list = mtx_list;
        break;
    }
    default: // serial
    {
        SerialLinkedList *serial_list = malloc(sizeof(SerialLinkedList));
        printf("Initializing serial list\n");
        serial_initialize_list(serial_list);
        printf("Populating serial list\n");
        populate_serial_list(serial_list, randNumbers, n);
        list = serial_list;
        break;
    }
    }

    pthread_t threads[t];
    ThreadArgs thread_args[t];
    int ops_per_thread = m / t;

    for (int i = 0; i < t; i++)
    {
        thread_args[i].list = list;
        thread_args[i].num_operations = ops_per_thread;
        thread_args[i].member_fraction = mMember;
        thread_args[i].insert_fraction = mInsert;
        thread_args[i].delete_fraction = mDelete;
        thread_args[i].type = type;
    }

    clock_t start = clock();

    printf("Creating %d threads\n", t);
    for (int i = 0; i < t; i++)
    {
        pthread_create(&threads[i], NULL, perform_operations, &thread_args[i]);
    }

    for (int i = 0; i < t; i++)
    {
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    switch (type)
    {
    case 1:
    {
        printf("Read-write lock implementation time: %f seconds\n", cpu_time_used);
        rwl_cleanup_list((RwlLinkedList *)list);
        break;
    }
    case 2:
    {
        printf("Mutex implementation time: %f seconds\n", cpu_time_used);
        mtx_cleanup_list((MtxLinkedList *)list);
        break;
    }
    default:
    {
        printf("Serial implementation time: %f seconds\n", cpu_time_used);
        serial_cleanup_list((SerialLinkedList *)list);
        break;
    }
    }
    free(list);
}

void printArray(int *arr, int size)
{
    printf("\n");
    printf("Array elements: ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        printf("Usage: %s <n> <m> <mMember> <mInsert> <mDelete> <t>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    double mMember = atof(argv[3]);
    double mInsert = atof(argv[4]);
    double mDelete = atof(argv[5]);
    int t = atoi(argv[6]);

    printf("Running tests with n=%d, m=%d, mMember=%.2f, mInsert=%.2f, mDelete=%.2f, t=%d\n",
           n, m, mMember, mInsert, mDelete, t);

    srand(time(NULL));

    int *randNumbers = malloc(sizeof(int) * n);
    generate_unique_random_numbers(randNumbers, n);

    run_test(1, n, m, mMember, mInsert, mDelete, t, randNumbers); // Read-write lock test
    // printArray(randNumbers, n);
    run_test(2, n, m, mMember, mInsert, mDelete, t, randNumbers); // Mutex test
    // printArray(randNumbers, n);
    if (t == 1)
    {
        run_test(3, n, m, mMember, mInsert, mDelete, t, randNumbers); // Serial test
    }
    // printArray(randNumbers, n);
    free(randNumbers);
    return 0;

    int thread_counts[] = {1, 2, 4, 8};
}
