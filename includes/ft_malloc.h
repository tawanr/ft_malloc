#ifndef __FT_MALLOC__
#define __FT_MALLOC__

#include <stdio.h>
#include <sys/mman.h>

void show_malloc();
void *ft_malloc(size_t size);
void ft_free(void *ptr);

#endif