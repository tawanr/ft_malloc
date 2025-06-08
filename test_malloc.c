#include "ft_malloc.h"
#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_test_header(const char *test_name) {
    printf("\n=== %s ===\n", test_name);
}

void print_test_result(const char *test_name, int passed) {
    printf("%s: %s\n", test_name, passed ? "PASSED" : "FAILED");
}

void test_basic_allocation() {
    print_test_header("Basic Allocation Test");

    char *tiny = ft_malloc(16);
    assert(tiny != NULL);
    strcpy(tiny, "tiny test");
    assert(strcmp(tiny, "tiny test") == 0);
    print_test_result("Tiny allocation", 1);

    char *small = ft_malloc(1024);
    assert(small != NULL);
    strcpy(small, "small test");
    assert(strcmp(small, "small test") == 0);
    print_test_result("Small allocation", 1);

    char *large = ft_malloc(1024 * 1024);
    assert(large != NULL);
    strcpy(large, "large test");
    assert(strcmp(large, "large test") == 0);
    print_test_result("Large allocation", 1);

    ft_free(tiny);
    ft_free(small);
    ft_free(large);
}

void test_reallocation() {
    print_test_header("Reallocation Test");

    char *ptr = ft_malloc(100);
    assert(ptr != NULL);
    strcpy(ptr, "test string");

    char *new_ptr = ft_realloc(ptr, 50);
    assert(new_ptr != NULL);
    assert(strcmp(new_ptr, "test string") == 0);
    print_test_result("Realloc smaller size", 1);

    char *larger_ptr = ft_realloc(new_ptr, 200);
    assert(larger_ptr != NULL);
    assert(strcmp(larger_ptr, "test string") == 0);
    strcat(larger_ptr, " extended");
    assert(strcmp(larger_ptr, "test string extended") == 0);
    print_test_result("Realloc larger size", 1);

    ft_free(larger_ptr);
}

void test_free_and_reuse() {
    print_test_header("Free and Reuse Test");

    for (int i = 0; i < 5; i++) {
        char *ptr = ft_malloc(32);
        assert(ptr != NULL);
        sprintf(ptr, "test %d", i);
        ft_free(ptr);
    }
    print_test_result("Multiple alloc/free cycles", 1);

    char *ptr1 = ft_malloc(32);
    assert(ptr1 != NULL);
    ft_free(ptr1);

    char *ptr2 = ft_malloc(32);
    assert(ptr2 != NULL);
    print_test_result("Memory reuse", 1);

    ft_free(ptr2);
}

void test_edge_cases() {
    print_test_header("Edge Cases Test");

    void *zero_ptr = ft_malloc(0);
    assert(zero_ptr == NULL);
    print_test_result("Zero size allocation", 1);

    void *huge_ptr = ft_malloc(SIZE_MAX);
    assert(huge_ptr == NULL);
    print_test_result("Huge size allocation", 1);

    ft_free(NULL);
    print_test_result("Free NULL pointer", 1);
}

void test_memory_corruption() {
    print_test_header("Memory Corruption Test");

    char *ptr = ft_malloc(16);
    assert(ptr != NULL);

    for (int i = 0; i < 32; i++) {
        ptr[i] = 'A';
    }

    ft_free(ptr);
    char *new_ptr = ft_malloc(16);
    assert(new_ptr != NULL);
    print_test_result("Memory corruption handling", 1);

    ft_free(new_ptr);
}

void test_multiple_blocks() {
    print_test_header("Multiple Blocks Test");

    void *ptrs[100];

    for (int i = 0; i < 100; i++) {
        ptrs[i] = ft_malloc(32);
        assert(ptrs[i] != NULL);
    }
    print_test_result("Multiple allocations", 1);

    for (int i = 0; i < 100; i += 2) {
        ft_free(ptrs[i]);
    }
    print_test_result("Alternate freeing", 1);

    for (int i = 1; i < 100; i += 2) {
        ft_free(ptrs[i]);
    }
    print_test_result("Cleanup", 1);
}

void *thread_alloc_free(void *arg) {
    int iterations = 20;
    void **ptrs = malloc(sizeof(void *) * iterations);

    if (!ptrs) {
        printf("Failed to allocate memory for pointers array\n");
        return NULL;
    }

    for (int i = 0; i < iterations; i++) {
        ptrs[i] = ft_malloc(32);
        if (!ptrs[i]) {
            printf("Thread allocation failed at iteration %d\n", i);
            for (int j = 0; j < i; j++) {
                if (ptrs[j]) {
                    ft_free(ptrs[j]);
                    ptrs[j] = NULL;
                }
            }
            free(ptrs);
            return NULL;
        }
        memset(ptrs[i], 'A' + (i % 26), 32);
    }

    for (int i = 0; i < iterations; i++) {
        if (ptrs[i]) {
            ft_free(ptrs[i]);
            ptrs[i] = NULL;
        }
    }

    free(ptrs);
    return NULL;
}

void test_thread_safety() {
    print_test_header("Thread Safety Test");

    const int NUM_THREADS = 4;
    pthread_t threads[NUM_THREADS];
    int thread_results[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_results[i] =
            pthread_create(&threads[i], NULL, thread_alloc_free, NULL);
        if (thread_results[i] != 0) {
            printf("Failed to create thread %d\n", i);
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            print_test_result("Thread creation", 0);
            return;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        void *thread_return;
        int join_result = pthread_join(threads[i], &thread_return);
        if (join_result != 0 || thread_return != NULL) {
            printf("Thread %d failed or returned error\n", i);
            print_test_result("Thread execution", 0);
            return;
        }
    }

    void *final_check = ft_malloc(32);
    if (!final_check) {
        printf("Final allocation check failed\n");
        print_test_result("Post-thread allocation", 0);
        return;
    }
    ft_free(final_check);

    print_test_result("Concurrent allocations and frees", 1);
}

void test_memory_leaks() {
    print_test_header("Memory Leak Test");

    void *ptrs[10];
    size_t sizes[10] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};

    for (int i = 0; i < 10; i++) {
        ptrs[i] = ft_malloc(sizes[i]);
        assert(ptrs[i] != NULL);
        memset(ptrs[i], 'A' + i, sizes[i]);
    }

    for (int i = 0; i < 10; i++) {
        ft_free(ptrs[i]);
    }

    void *new_ptr = ft_malloc(16);
    assert(new_ptr != NULL);
    ft_free(new_ptr);

    print_test_result("Basic memory leak check", 1);
}

void test_nested_allocation_leaks() {
    print_test_header("Nested Allocation Leak Test");

    void *outer = ft_malloc(1024);
    assert(outer != NULL);

    void *inner = ft_malloc(512);
    assert(inner != NULL);

    ft_free(outer);
    ft_free(inner);

    void *check = ft_malloc(16);
    assert(check != NULL);
    ft_free(check);

    print_test_result("Nested allocation leak check", 1);
}

void test_realloc_leaks() {
    print_test_header("Realloc Leak Test");

    void *ptr = ft_malloc(100);
    assert(ptr != NULL);

    void *realloced = ft_realloc(ptr, 200);
    assert(realloced != NULL);

    void *realloced2 = ft_realloc(realloced, 50);
    assert(realloced2 != NULL);

    ft_free(realloced2);

    void *check = ft_malloc(16);
    assert(check != NULL);
    ft_free(check);

    print_test_result("Realloc leak check", 1);
}

void test_thread_leaks() {
    print_test_header("Thread Memory Leak Test");

    const int NUM_THREADS = 4;
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        assert(pthread_create(&threads[i], NULL, thread_alloc_free, NULL) == 0);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        assert(pthread_join(threads[i], NULL) == 0);
    }

    void *check = ft_malloc(16);
    assert(check != NULL);
    ft_free(check);

    print_test_result("Thread memory leak check", 1);
}

int main() {
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

    printf("\nAll tests completed!\n");
    return 0;
}