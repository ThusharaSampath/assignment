#include "mtx_linked_list.h"
#include "rwl_linked_list.h"
#include "serial_linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#define MAX_VALUE 65535 // 2^16 - 1
#define MAX_THREAD_COUNT 4
#define CONFIDENTIAL_INTERVAL_Z 1.96
#define ACCURACY_R 5

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

double run_test(int type, int n, int m, double mMember, double mInsert, double mDelete, int t, int *randNumbers)
{
    void *list;
    switch (type)
    {
    case 1: // read write lock
    {
        RwlLinkedList *rwl_list = malloc(sizeof(RwlLinkedList));
        rwl_initialize_list(rwl_list);
        populate_rwl_list(rwl_list, randNumbers, n);
        list = rwl_list;
        break;
    }
    case 2: // mutex
    {
        MtxLinkedList *mtx_list = malloc(sizeof(MtxLinkedList));
        mtx_initialize_list(mtx_list);
        populate_mtx_list(mtx_list, randNumbers, n);
        list = mtx_list;
        break;
    }
    default: // serial
    {
        SerialLinkedList *serial_list = malloc(sizeof(SerialLinkedList));
        serial_initialize_list(serial_list);
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

    // Start measuring time here
    clock_t start = clock();

    for (int i = 0; i < t; i++)
    {
        pthread_create(&threads[i], NULL, perform_operations, &thread_args[i]);
    }

    for (int i = 0; i < t; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Stop measuring time here
    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    switch (type)
    {
    case 1:
    {
        rwl_cleanup_list((RwlLinkedList *)list);
        break;
    }
    case 2:
    {
        mtx_cleanup_list((MtxLinkedList *)list);
        break;
    }
    default:
    {
        serial_cleanup_list((SerialLinkedList *)list);
        break;
    }
    }
    free(list);

    return cpu_time_used;
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

double calculate_average(double *values, int count)
{
    double sum = 0;
    for (int i = 0; i < count; i++)
    {
        sum += values[i];
    }
    return sum / count;
}

double calculate_std_dev(double *values, int count, double average)
{
    double sum_squared_diff = 0;
    for (int i = 0; i < count; i++)
    {
        double diff = values[i] - average;
        sum_squared_diff += diff * diff;
    }
    return sqrt(sum_squared_diff / count);
}

int calculate_minimum_sample_count(double s, double x)
{
    double z = CONFIDENTIAL_INTERVAL_Z;
    double r = ACCURACY_R;
    return (int)(((100 * z * s * 100 * z * s) / (r * x * r * x)));
}

void run_test_suite(int n, int m, double mMember, double mInsert, double mDelete, int sample_count)
{
    int thread_counts[] = {1, 2, 4, 8};
    int num_thread_counts = sizeof(thread_counts) / sizeof(thread_counts[0]);

    printf("Running tests with n=%d, m=%d, mMember=%.3f, mInsert=%.3f, mDelete=%.3f, sample_count=%d\n",
           n, m, mMember, mInsert, mDelete, sample_count);
    printf("|-----------------|---------------|---------|--------|-------------------|\n");
    printf("|  Implementation | No of threads | Average |   Std  |  Min sample count |\n");
    printf("|-----------------|---------------|---------|--------|-------------------|\n");

    for (int impl = 1; impl <= 3; impl++)
    {
        const char *impl_name;
        switch (impl)
        {
        case 1:
            impl_name = "Read-Write lock";
            break;
        case 2:
            impl_name = "Mutex";
            break;
        case 3:
            impl_name = "Serial";
            break;
        }

        for (int t_idx = 0; t_idx < num_thread_counts; t_idx++)
        {
            int t = thread_counts[t_idx];
            if (impl == 3 && t > 1)
                continue; // Skip serial implementation for thread counts > 1

            double *times = malloc(sizeof(double) * sample_count);

            for (int sample = 0; sample < sample_count; sample++)
            {
                int *randNumbers = malloc(sizeof(int) * n);
                generate_unique_random_numbers(randNumbers, n);

                times[sample] = run_test(impl, n, m, mMember, mInsert, mDelete, t, randNumbers);

                free(randNumbers);
            }

            double avg = calculate_average(times, sample_count);
            double std_dev = calculate_std_dev(times, sample_count, avg);
            int min_sample_count = calculate_minimum_sample_count(std_dev, avg);

            printf("| %-15s | %-13d | %-7.4f | %-3.4f | %-17d |\n", impl_name, t, avg, std_dev, min_sample_count);
            printf("|-----------------|---------------|---------|--------|-------------------|\n");
            free(times);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 7)
    {
        printf("Usage: %s <n> <m> <mMember> <mInsert> <mDelete> <sample_count>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    double mMember = atof(argv[3]);
    double mInsert = atof(argv[4]);
    double mDelete = atof(argv[5]);
    int sample_count = atoi(argv[6]);

    srand(time(NULL));

    run_test_suite(n, m, mMember, mInsert, mDelete, sample_count);

    return 0;
}