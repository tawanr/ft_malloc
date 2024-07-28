#include "ft_malloc.h"

int main()
{
    void *mem = ft_malloc(5);
    show_malloc();
    void *mem2 = ft_malloc(10);
    show_malloc();
    ft_free(mem);
    show_malloc();
    ft_free(mem2);
    show_malloc();
    return 0;
}