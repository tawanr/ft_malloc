#include "malloc.h"
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_test_header(const char *test_name)
{
    printf("\n=== %s ===\n", test_name);
}

void print_test_result(const char *test_name, int passed)
{
    printf("%s: %s\n", test_name, passed ? "PASSED" : "FAILED");
}

void test_basic_allocation()
{
    print_test_header("Basic Allocation Test");

    char *tiny = malloc(16);
    assert(tiny != NULL);
    strcpy(tiny, "tiny test");
    assert(strcmp(tiny, "tiny test") == 0);
    print_test_result("Tiny allocation", 1);

    char *small = malloc(1024);
    assert(small != NULL);
    strcpy(small, "small test");
    assert(strcmp(small, "small test") == 0);
    print_test_result("Small allocation", 1);

    char *large = malloc(1024 * 1024);
    assert(large != NULL);
    strcpy(large, "large test");
    assert(strcmp(large, "large test") == 0);
    print_test_result("Large allocation", 1);

    free(tiny);
    free(small);
    free(large);
}

void test_reallocation()
{
    print_test_header("Reallocation Test");

    char *ptr = malloc(100);
    assert(ptr != NULL);
    strcpy(ptr, "test string");

    char *new_ptr = realloc(ptr, 50);
    assert(new_ptr != NULL);
    assert(strcmp(new_ptr, "test string") == 0);
    print_test_result("Realloc smaller size", 1);

    char *larger_ptr = realloc(new_ptr, 200);
    assert(larger_ptr != NULL);
    assert(strcmp(larger_ptr, "test string") == 0);
    strcat(larger_ptr, " extended");
    assert(strcmp(larger_ptr, "test string extended") == 0);
    print_test_result("Realloc larger size", 1);

    free(larger_ptr);
}

void test_free_and_reuse()
{
    print_test_header("Free and Reuse Test");

    for (int i = 0; i < 5; i++)
    {
        char *ptr = malloc(32);
        assert(ptr != NULL);
        sprintf(ptr, "test %d", i);
        free(ptr);
    }
    print_test_result("Multiple alloc/free cycles", 1);

    char *ptr1 = malloc(32);
    assert(ptr1 != NULL);
    free(ptr1);

    char *ptr2 = malloc(32);
    assert(ptr2 != NULL);
    print_test_result("Memory reuse", 1);

    free(ptr2);
}

void test_edge_cases()
{
    print_test_header("Edge Cases Test");

    void *zero_ptr = malloc(0);
    assert(zero_ptr == NULL);
    print_test_result("Zero size allocation", 1);

    void *huge_ptr = malloc(SIZE_MAX);
    assert(huge_ptr == NULL);
    print_test_result("Huge size allocation", 1);

    free(NULL);
    print_test_result("Free NULL pointer", 1);
}

void test_memory_corruption()
{
    print_test_header("Memory Corruption Test");

    char *ptr = malloc(16);
    assert(ptr != NULL);

    for (int i = 0; i < 32; i++)
    {
        ptr[i] = 'A';
    }

    free(ptr);
    char *new_ptr = malloc(16);
    assert(new_ptr != NULL);
    print_test_result("Memory corruption handling", 1);

    free(new_ptr);
}

void test_multiple_blocks()
{
    print_test_header("Multiple Blocks Test");

    void *ptrs[100];

    for (int i = 0; i < 100; i++)
    {
        ptrs[i] = malloc(32);
        assert(ptrs[i] != NULL);
    }
    print_test_result("Multiple allocations", 1);

    for (int i = 0; i < 100; i += 2)
    {
        free(ptrs[i]);
    }
    print_test_result("Alternate freeing", 1);

    for (int i = 1; i < 100; i += 2)
    {
        free(ptrs[i]);
    }
    print_test_result("Cleanup", 1);
}

void *thread_alloc_free(void *arg)
{
    int iterations = 20;
    void **ptrs = malloc(sizeof(void *) * iterations);

    if (!ptrs)
    {
        printf("Failed to allocate memory for pointers array\n");
        return NULL;
    }

    for (int i = 0; i < iterations; i++)
    {
        ptrs[i] = malloc(32);
        if (!ptrs[i])
        {
            printf("Thread allocation failed at iteration %d\n", i);
            for (int j = 0; j < i; j++)
            {
                if (ptrs[j])
                {
                    free(ptrs[j]);
                    ptrs[j] = NULL;
                }
            }
            free(ptrs);
            return NULL;
        }
        memset(ptrs[i], 'A' + (i % 26), 32);
    }

    for (int i = 0; i < iterations; i++)
    {
        if (ptrs[i])
        {
            free(ptrs[i]);
            ptrs[i] = NULL;
        }
    }

    free(ptrs);
    return NULL;
}

void test_thread_safety()
{
    print_test_header("Thread Safety Test");

    const int NUM_THREADS = 4;
    pthread_t threads[NUM_THREADS];
    int thread_results[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_results[i] =
            pthread_create(&threads[i], NULL, thread_alloc_free, NULL);
        if (thread_results[i] != 0)
        {
            printf("Failed to create thread %d\n", i);
            for (int j = 0; j < i; j++)
            {
                pthread_join(threads[j], NULL);
            }
            print_test_result("Thread creation", 0);
            return;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        void *thread_return;
        int join_result = pthread_join(threads[i], &thread_return);
        if (join_result != 0 || thread_return != NULL)
        {
            printf("Thread %d failed or returned error\n", i);
            print_test_result("Thread execution", 0);
            return;
        }
    }

    void *final_check = malloc(32);
    if (!final_check)
    {
        printf("Final allocation check failed\n");
        print_test_result("Post-thread allocation", 0);
        return;
    }
    free(final_check);

    print_test_result("Concurrent allocations and frees", 1);
}

void test_memory_leaks()
{
    print_test_header("Memory Leak Test");

    void *ptrs[10];
    size_t sizes[10] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};

    for (int i = 0; i < 10; i++)
    {
        ptrs[i] = malloc(sizes[i]);
        assert(ptrs[i] != NULL);
        memset(ptrs[i], 'A' + i, sizes[i]);
    }

    for (int i = 0; i < 10; i++)
    {
        free(ptrs[i]);
    }

    void *new_ptr = malloc(16);
    assert(new_ptr != NULL);
    free(new_ptr);

    print_test_result("Basic memory leak check", 1);
}

void test_nested_allocation_leaks()
{
    print_test_header("Nested Allocation Leak Test");

    void *outer = malloc(1024);
    assert(outer != NULL);

    void *inner = malloc(512);
    assert(inner != NULL);

    free(outer);
    free(inner);

    void *check = malloc(16);
    assert(check != NULL);
    free(check);

    print_test_result("Nested allocation leak check", 1);
}

void test_realloc_leaks()
{
    print_test_header("Realloc Leak Test");

    void *ptr = malloc(100);
    assert(ptr != NULL);

    void *realloced = realloc(ptr, 200);
    assert(realloced != NULL);

    void *realloced2 = realloc(realloced, 50);
    assert(realloced2 != NULL);

    free(realloced2);

    void *check = malloc(16);
    assert(check != NULL);
    free(check);

    print_test_result("Realloc leak check", 1);
}

void test_thread_leaks()
{
    print_test_header("Thread Memory Leak Test");

    const int NUM_THREADS = 4;
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++)
    {
        assert(pthread_create(&threads[i], NULL, thread_alloc_free, NULL) == 0);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        assert(pthread_join(threads[i], NULL) == 0);
    }

    void *check = malloc(16);
    assert(check != NULL);
    free(check);

    print_test_result("Thread memory leak check", 1);
}

void test_memory_alignment()
{
    print_test_header("Memory Alignment Test");

    const size_t sizes[] = {1, 8, 10, 16, 32, 64,
                            128, 130, 256, 512, 1024, 2048};
    const size_t ALIGNMENT = 8;

    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
    {
        void *ptr = malloc(sizes[i]);
        if (!ptr)
        {
            printf("Allocation failed for size %zu\n", sizes[i]);
            print_test_result("Memory allocation", 0);
            return;
        }

        uintptr_t addr = (uintptr_t)ptr;
        if (addr % ALIGNMENT != 0)
        {
            printf(
                "Memory not aligned to %zu bytes for size %zu (address: %p)\n",
                ALIGNMENT, sizes[i], ptr);
            free(ptr);
            print_test_result("Memory alignment", 0);
            return;
        }

        free(ptr);
    }

    print_test_result("Memory alignment check", 1);
}

int main()
{
    printf("Starting malloc test suite...\n");

    test_basic_allocation();
    test_reallocation();
    test_free_and_reuse();
    test_edge_cases();
    test_memory_corruption();
    test_multiple_blocks();
    test_thread_safety();
    test_memory_leaks();
    test_nested_allocation_leaks();
    test_realloc_leaks();
    test_thread_leaks();
    test_memory_alignment();

    printf("\nAll tests completed!\n");
    return 0;
}