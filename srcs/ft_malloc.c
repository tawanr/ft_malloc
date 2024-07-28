#include "ft_malloc.h"

MemoryBlocks blocks = {NULL, NULL, NULL};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

MemoryNode *allocate_new_block(MemoryNode **head, size_t block_size)
{
    MemoryNode *cur = *head;
    MemoryNode *new_block = mmap(NULL, sizeof(MemoryNode), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (new_block == NULL)
        return NULL;
    new_block->loc = mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (new_block->loc == NULL)
        return NULL;

    new_block->is_free = 1;
    new_block->size = block_size;
    new_block->next = NULL;
    new_block->prev = NULL;
    if (cur == NULL)
    {
        *head = new_block;
        return new_block;
    }
    while (cur->next != NULL)
        cur = cur->next;
    cur->next = new_block;
    new_block->prev = cur;
    return new_block;
}

void get_block_limit(size_t size, MemoryNode ***head, size_t *resolution, size_t *block_size)
{
    const size_t TINY_MAX = 992;
    const size_t TINY_SIZE = 2 * 1024 * 1024;
    const size_t TINY_RESOLUTION = 16;

    const size_t SMALL_MAX = 127 * 1024;
    const size_t SMALL_SIZE = 16 * 1024 * 1024;
    const size_t SMALL_RESOLUTION = 512;

    const size_t LARGE_RESOLUTION = 4 * 1024;

    if (size <= TINY_MAX)
    {
        *resolution = TINY_RESOLUTION;
        *block_size = TINY_SIZE;
        *head = &(blocks.tiny_head);
    }
    else if (size < SMALL_MAX)
    {
        *resolution = SMALL_RESOLUTION;
        *block_size = SMALL_SIZE;
        *head = &(blocks.small_head);
    }
    else
    {
        *resolution = LARGE_RESOLUTION;
        *head = &(blocks.large_head);
    }
}

void *allocate_zone(size_t size)
{
    MemoryNode **head = NULL;
    MemoryNode *rtn = NULL;
    size_t resolution = 0;
    size_t block_size = 0;

    get_block_limit(size, &head, &resolution, &block_size);

    MemoryNode *cur = *head;

    if (size < resolution)
        size = resolution;
    if (size > block_size)
        block_size = size;
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
    if (rtn == NULL)
    {
        if (allocate_new_block(head, block_size) == NULL)
            return rtn;
        rtn = allocate_zone(size);
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

void show_alloc_mem()
{
    size_t total = 0;

    MemoryNode *cur = blocks.tiny_head;
    ft_printf("TINY : 0x%X\n", cur);
    while (cur != NULL)
    {
        if (cur->is_free == 0)
        {
            ft_printf("0x%X - 0x%X : %u bytes\n", cur->loc, cur->loc + cur->size, cur->size);
            total += cur->size;
        }
        cur = cur->next;
    }

    cur = blocks.small_head;
    ft_printf("SMALL : 0x%X\n", cur);
    while (cur != NULL)
    {
        if (cur->is_free == 0)
        {
            ft_printf("0x%X - 0x%X : %u bytes\n", cur->loc, cur->loc + cur->size, cur->size);
            total += cur->size;
        }
        cur = cur->next;
    }

    cur = blocks.large_head;
    ft_printf("LARGE : 0x%X\n", cur);
    while (cur != NULL)
    {
        if (cur->is_free == 0)
        {
            ft_printf("0x%X - 0x%X : %u bytes\n", cur->loc, cur->loc + cur->size, cur->size);
            total += cur->size;
        }
        cur = cur->next;
    }
    ft_printf("Total : %u bytes\n", total);
}

void check_head_free(MemoryNode *block)
{
    if (blocks.tiny_head == block)
    {
        munmap(block->loc, block->size);
        munmap(block, sizeof(MemoryNode));
        blocks.tiny_head = NULL;
    }
    else if (blocks.small_head == block)
    {
        munmap(block->loc, block->size);
        munmap(block, sizeof(MemoryNode));
        blocks.small_head = NULL;
    }
    else if (blocks.large_head == block)
    {
        munmap(block->loc, block->size);
        munmap(block, sizeof(MemoryNode));
        blocks.large_head = NULL;
    }
}

void free_block(MemoryNode *block)
{
    MemoryNode *temp = NULL;

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
        check_head_free(block);
}

void ft_free(void *ptr)
{
    pthread_mutex_lock(&lock);
    MemoryNode *block = get_block(ptr);
    if (block == NULL)
        return;
    free_block(block);
    pthread_mutex_unlock(&lock);
}

void *ft_malloc(size_t size)
{
    MemoryNode *allocated = NULL;

    pthread_mutex_lock(&lock);
    allocated = allocate_zone(size);
    pthread_mutex_unlock(&lock);

    return allocated->loc;
}

void *ft_realloc(void *ptr, size_t size)
{
    pthread_mutex_lock(&lock);
    MemoryNode *block = get_block(ptr);
    if (block == NULL)
    {
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    size_t prev_size;
    if (size < block->size)
    {
        prev_size = block->size;
        block->size = size;
        if (block->next != NULL && block->next->is_free == 1)
        {
            block->next->size += block->size;
            pthread_mutex_unlock(&lock);
            return block->loc;
        }
        MemoryNode *new_block = mmap(NULL, sizeof(MemoryNode), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        new_block->size = prev_size - size;
        new_block->is_free = 1;
        new_block->prev = block;
        new_block->next = block->next;
        new_block->loc = block->loc + block->size;
        pthread_mutex_unlock(&lock);
        return new_block;
    }
    MemoryNode *allocated = allocate_zone(size);
    if (allocated == NULL)
    {
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    ft_strlcpy(allocated->loc, block->loc, block->size);
    free_block(block);
    pthread_mutex_unlock(&lock);
    return allocated->loc;
}