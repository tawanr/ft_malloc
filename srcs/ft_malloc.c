#include "ft_malloc.h"
#include "mem_blocks.h"
#include "global_mem.h"

MemoryBlocks blocks = {NULL, NULL, NULL};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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
        if (rtn == NULL || rtn == MAP_FAILED)
            return NULL;
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

void *ft_malloc(size_t size)
{
    MemoryNode *allocated = NULL;

    pthread_mutex_lock(&lock);
    allocated = allocate_zone(size);
    pthread_mutex_unlock(&lock);

    return allocated->loc;
}
