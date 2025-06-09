#include "ft_malloc.h"

int main() {
    printf("page size: %d\n", getpagesize());
    char *mem = ft_malloc(16);
    show_alloc_mem();
    printf("\n");
    mem = ft_realloc(mem, 20);
    ft_strlcpy(mem, "testing", 20);
    printf("mem: %s\n", mem);
    show_alloc_mem();
    printf("\n");
    char *mem2 = ft_realloc(mem, 50);
    printf("mem2: %s len: %lu\n", mem2, ft_strlen(mem2));
    printf("mem: %p mem2: %p\n", mem, mem2);
    show_alloc_mem();
    printf("\n");
    void *tiny = ft_malloc(1024);
    void *small = ft_malloc(2048);
    void *small2 = ft_malloc(3400);
    void *big = ft_malloc(512000);
    show_alloc_mem();
    printf("\n");
    big = ft_realloc(big, 600000);
    small = ft_realloc(small, 600);
    // void *mem2 = ft_malloc(10);
    // show_alloc_mem();
    // ft_free(mem);
    // show_alloc_mem();
    ft_free(mem2);
    show_alloc_mem();
    printf("\n");
    ft_free(small);
    show_alloc_mem();
    printf("\n");
    return 0;
}