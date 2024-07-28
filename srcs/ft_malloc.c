#include "ft_malloc.h"
#include <stdlib.h>

typedef struct MemoryNode
{
    struct MemoryNode *next;
    struct MemoryNode *prev;
    void *loc;
    size_t size;
    int is_free;
} MemoryNode;

typedef struct MemoryBlocks
{
    MemoryNode *tiny_head;
    MemoryNode *small_head;
    MemoryNode *large_head;
} MemoryBlocks;

MemoryBlocks blocks = {NULL, NULL, NULL};

void *allocate_zone(size_t size, MemoryNode **head, size_t resolution)
{
    MemoryNode *cur = *head;
    MemoryNode *rtn = NULL;
    if (size < resolution)
        size = resolution;
    while (cur != NULL)
    {
        if ((cur->is_free == 0) || (cur->size < size))
        {
            cur = cur->next;
            continue;
        }
        rtn = mmap(NULL, sizeof(MemoryNode), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        rtn->loc = cur->loc;
        rtn->size = size;
        rtn->is_free = 0;
        rtn->prev = cur->prev;
        rtn->next = cur;

        if (rtn->prev != NULL)
            rtn->prev->next = rtn;

        cur->prev = rtn;
        cur->size = cur->size - size;
        cur->loc = cur->loc + size;

        if (cur == *head)
            *head = rtn;
        return rtn;
    }
    return rtn;
}

MemoryNode *get_block(void *loc)
{
    MemoryNode *cur = blocks.tiny_head;
    while (cur != NULL)
    {
        if (cur->loc == loc)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void show_malloc()
{
    MemoryNode *cur = blocks.tiny_head;
    if (cur == NULL)
        printf("Block is empty\n");
    while (cur != NULL)
    {
        printf("size: %lu free: %d\n", cur->size, cur->is_free);
        cur = cur->next;
    }
}

void ft_free(void *ptr)
{
    MemoryNode *block = get_block(ptr);
    MemoryNode *temp = NULL;
    if (block == NULL)
        return;
    block->is_free = 1;
    if ((block->next != NULL) && (block->next->is_free == 1))
    {
        temp = block->next;
        block->size += block->next->size;
        block->next = block->next->next;
        munmap(temp, sizeof(MemoryNode));
    }
    if ((block->prev != NULL) && (block->prev->is_free == 1))
    {
        block->prev->next = block->next;
        block->prev->size += block->size;
        if (block->next != NULL)
            block->next->prev = block->prev;
        temp = block;
        block = block->prev;
        munmap(temp, sizeof(MemoryNode));
    }
    if (block->prev == NULL && block->next == NULL)
    {
        if (blocks.tiny_head == block)
        {
            munmap(block->loc, block->size);
            munmap(block, sizeof(MemoryNode));
            blocks.tiny_head = NULL;
        }
    }
}

void *ft_malloc(size_t size)
{
    const size_t TINY_MAX = 992;
    const size_t TINY_SIZE = 2 * 1024 * 1024;
    const size_t TINY_RESOLUTION = 16;

    MemoryNode *allocated = NULL;

    if (size > TINY_MAX)
    {
        return NULL;
    }
    if (blocks.tiny_head == NULL)
    {
        MemoryNode *ptr = mmap(NULL, sizeof(MemoryNode), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (ptr == NULL)
        {
            printf("Cannot allocate\n");
        }
        ptr->loc = mmap(NULL, TINY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (ptr->loc == NULL)
        {
            printf("Cannot allocate\n");
        }
        ptr->next = NULL;
        ptr->prev = NULL;
        ptr->size = TINY_SIZE;
        ptr->is_free = 1;
        blocks.tiny_head = ptr;
    }
    allocated = allocate_zone(size, &(blocks.tiny_head), TINY_RESOLUTION);

    if (allocated == NULL)
        return NULL;

    return allocated->loc;
}