#ifndef __FT_MALLOC__
#define __FT_MALLOC__

#include <stdio.h>
#include <sys/mman.h>
#include "libft.h"

void show_alloc_mem();
void *ft_malloc(size_t size);
void ft_free(void *ptr);
void *ft_realloc(void *ptr, size_t size);

#endif