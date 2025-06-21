#include "malloc.h"
#include "global_mem.h"
#include "mem_blocks.h"
#include <stdint.h>

MemoryBlocks blocks = {NULL, NULL, NULL, NULL};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static NodePool *get_node_pool()
{
    if (blocks.node_pool == NULL)
        blocks.node_pool = init_node_pool(NODE_POOL_SIZE);
    return blocks.node_pool;
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
    if (size > resolution && size % resolution != 0)
        size = size + ((resolution - (size % resolution)));
    while (cur != NULL)
    {
        if ((cur->is_free == 0) || (cur->size < size))
        {
            cur = cur->next;
            continue;
        }
        rtn = get_node_from_pool(get_node_pool());
        if (rtn == NULL)
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

void *malloc(size_t size)
{
    if (size == 0 || size > SIZE_MAX - sizeof(void *))
        return NULL;

    MemoryNode *allocated = NULL;

    pthread_mutex_lock(&lock);
    allocated = allocate_zone(size);
    pthread_mutex_unlock(&lock);

    if (allocated == NULL)
        return NULL;

    return allocated->loc;
}
