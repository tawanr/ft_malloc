#include "ft_malloc.h"

int main()
{
    printf("page size: %d\n", getpagesize());
    void *mem = ft_malloc(5);
    show_alloc_mem();
    void *mem2 = ft_malloc(10);
    show_alloc_mem();
    ft_free(mem);
    show_alloc_mem();
    ft_free(mem2);
    show_alloc_mem();
    return 0;
}