Introduction:

This program generates a performance report for three types of linked list implementations:
1. Read-Write lock based linked list
2. Mutex based linked list
3. Serial linked list

Sample output:

Running tests with n=1000, m=10000, mMember=0.50, mInsert=0.25, mDelete=0.25, sample_count=500
|-----------------|---------------|---------|--------|-------------------|
|  Implementation | No of threads | Average |   Std  |  Min sample count |
|-----------------|---------------|---------|--------|-------------------|
| Read-Write lock | 1             | 0.0224  | 0.0004 | 0                 |
|-----------------|---------------|---------|--------|-------------------|
| Read-Write lock | 2             | 0.0396  | 0.0029 | 8                 |
|-----------------|---------------|---------|--------|-------------------|
| Read-Write lock | 4             | 0.0504  | 0.0025 | 3                 |
|-----------------|---------------|---------|--------|-------------------|
| Read-Write lock | 8             | 0.0519  | 0.0071 | 28                |
|-----------------|---------------|---------|--------|-------------------|
| Mutex           | 1             | 0.0225  | 0.0004 | 0                 |
|-----------------|---------------|---------|--------|-------------------|
| Mutex           | 2             | 0.0366  | 0.0017 | 3                 |
|-----------------|---------------|---------|--------|-------------------|
| Mutex           | 4             | 0.0617  | 0.0101 | 41                |
|-----------------|---------------|---------|--------|-------------------|
| Mutex           | 8             | 0.1174  | 0.0269 | 80                |
|-----------------|---------------|---------|--------|-------------------|
| Serial          | 1             | 0.0222  | 0.0003 | 0                 |
|-----------------|---------------|---------|--------|-------------------|

Usage instructions:

Step 1: Compile the Program

gcc -O2 -pthread -o linked_list_test mtx_linked_list.c rwl_linked_list.c linked_list_driver.c serial_linked_list.c

Step 2: Run the Program

./linked_list_test <n> <m> <mMember> <mInsert> <mDelete> <sample_count>

where,
- n: Number of initial nodes in the linked list.
- m: Number of operations to perform on the linked list.
- mMember: Fraction of "Member" operations.
- mInsert: Fraction of "Insert" operations.
- mDelete: Fraction of "Delete" operations.
- sample_count: Number of times the test should be repeated for each configuration.
