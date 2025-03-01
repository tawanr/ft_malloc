#ifndef __FT_MALLOC__
#define __FT_MALLOC__

#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include "libft.h"

typedef struct MemoryNode
{
    struct MemoryNode *next;
    struct MemoryNode *prev;
    void *loc;
    size_t size;
    int is_free;
} MemoryNode;

typedef struct BlockLimits
{
    size_t max_size;
    size_t resolution;
    size_t block_size;
} BlockLimits;

typedef struct MemoryBlocks
{
    MemoryNode *tiny_head;
    MemoryNode *small_head;
    MemoryNode *large_head;
} MemoryBlocks;

void show_alloc_mem();
void *ft_malloc(size_t size);
void ft_free(void *ptr);
void *ft_realloc(void *ptr, size_t size);

MemoryBlocks blocks = {NULL, NULL, NULL};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

#endif