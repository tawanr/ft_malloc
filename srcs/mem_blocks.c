/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_blocks.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 13:53:31 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 22:53:38 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "global_mem.h"

MemoryNode *allocate_new_block(MemoryNode **head, size_t block_size)
{
    MemoryNode *cur = *head;

    if (blocks.node_pool == NULL)
    {
        blocks.node_pool = init_node_pool(NODE_POOL_SIZE);
        if (blocks.node_pool == NULL)
            return NULL;
    }

    MemoryNode *new_block = get_node_from_pool(blocks.node_pool);
    if (new_block == NULL)
        return NULL;
    new_block->loc = mmap(NULL, block_size, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANON, -1, 0);
    if (new_block->loc == NULL || new_block->loc == MAP_FAILED)
    {
        return_node_to_pool(blocks.node_pool, new_block);
        return NULL;
    }

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

void get_block_limit(size_t size, MemoryNode ***head, size_t *resolution,
                     size_t *block_size)
{
    const size_t PAGE_SIZE = (size_t)(getpagesize());

    const size_t TINY_RESOLUTION = PAGE_SIZE / 256;
    const size_t TINY_MAX = 64 * TINY_RESOLUTION;
    const size_t TINY_SIZE = 16 * PAGE_SIZE;

    const size_t SMALL_RESOLUTION = PAGE_SIZE / 8;
    const size_t SMALL_MAX = 256 * SMALL_RESOLUTION;
    const size_t SMALL_SIZE = 64 * PAGE_SIZE;

    const size_t LARGE_RESOLUTION = PAGE_SIZE;

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
        *block_size = size;
        *head = &(blocks.large_head);
    }
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
    cur = blocks.small_head;
    while (cur != NULL)
    {
        if (cur->loc == loc)
            return cur;
        cur = cur->next;
    }
    cur = blocks.large_head;
    while (cur != NULL)
    {
        if (cur->loc == loc)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void check_head_free(MemoryNode *block)
{
    if (blocks.tiny_head == block)
    {
        munmap(block->loc, block->size);
        return_node_to_pool(blocks.node_pool, block);
        blocks.tiny_head = NULL;
    }
    else if (blocks.small_head == block)
    {
        munmap(block->loc, block->size);
        return_node_to_pool(blocks.node_pool, block);
        blocks.small_head = NULL;
    }
    else if (blocks.large_head == block)
    {
        munmap(block->loc, block->size);
        return_node_to_pool(blocks.node_pool, block);
        blocks.large_head = NULL;
    }
}

int free_block(MemoryNode *block)
{
    MemoryNode *temp = NULL;
    int rtn = 0;

    block->is_free = 1;
    if ((block->next != NULL) && (block->next->is_free == 1))
    {
        temp = block->next;
        block->size += block->next->size;
        block->next = block->next->next;
        return_node_to_pool(blocks.node_pool, temp);
    }
    if ((block->prev != NULL) && (block->prev->is_free == 1))
    {
        block->prev->next = block->next;
        block->prev->size += block->size;
        if (block->next != NULL)
            block->next->prev = block->prev;
        temp = block;
        block = block->prev;
        return_node_to_pool(blocks.node_pool, temp);
    }
    if (block->prev == NULL && block->next == NULL)
        check_head_free(block);
    return rtn;
}