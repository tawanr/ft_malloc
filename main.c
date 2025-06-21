#include "malloc.h"

int main()
{
    printf("page size: %d\n", getpagesize());
    char *mem = malloc(16);
    show_alloc_mem();
    printf("\n");
    mem = realloc(mem, 20);
    ft_strlcpy(mem, "testing", 20);
    printf("mem: %s\n", mem);
    show_alloc_mem();
    printf("\n");
    char *mem2 = realloc(mem, 50);
    printf("mem2: %s len: %lu\n", mem2, ft_strlen(mem2));
    printf("mem: %p mem2: %p\n", mem, mem2);
    show_alloc_mem();
    printf("\n");
    void *tiny = malloc(1024);
    void *small = malloc(2048);
    void *small2 = malloc(3400);
    void *big = malloc(512000);
    show_alloc_mem();
    printf("\n");
    big = realloc(big, 600000);
    small = realloc(small, 600);
    // void *mem2 = malloc(10);
    // show_alloc_mem();
    // free(mem);
    // show_alloc_mem();
    free(mem2);
    show_alloc_mem();
    printf("\n");
    free(small);
    show_alloc_mem();
    printf("\n");
    return 0;
}