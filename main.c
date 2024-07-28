#include "ft_malloc.h"

int main()
{
    printf("page size: %d\n", getpagesize());
    char *mem = ft_malloc(20);
    ft_strlcpy(mem, "testing", 20);
    printf("mem: %s\n", mem);
    char *mem2 = ft_realloc(mem, 50);
    printf("mem2: %s len: %lu\n", mem2, ft_strlen(mem2));
    printf("mem: %p mem2: %p\n", mem, mem2);
    show_alloc_mem();
    // void *mem2 = ft_malloc(10);
    // show_alloc_mem();
    // ft_free(mem);
    // show_alloc_mem();
    // ft_free(mem2);
    // show_alloc_mem();
    return 0;
}