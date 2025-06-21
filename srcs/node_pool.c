/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_pool.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 19:30:00 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 22:35:52 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "global_mem.h"

NodePool *init_node_pool(size_t capacity)
{
    NodePool *pool = mmap(NULL, sizeof(NodePool), PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANON, -1, 0);
    if (pool == NULL || pool == MAP_FAILED)
        return NULL;

    pool->pool = mmap(NULL, capacity * sizeof(MemoryNode), PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANON, -1, 0);
    if (pool->pool == NULL || pool->pool == MAP_FAILED)
    {
        munmap(pool, sizeof(NodePool));
        return NULL;
    }

    pool->free_list = pool->pool;
    pool->used = 0;
    pool->capacity = capacity;

    for (size_t i = 0; i < capacity - 1; i++)
    {
        pool->pool[i].next = &pool->pool[i + 1];
        pool->pool[i].prev = NULL;
        pool->pool[i].loc = NULL;
        pool->pool[i].size = 0;
        pool->pool[i].is_free = 0;
    }
    pool->pool[capacity - 1].next = NULL;
    pool->pool[capacity - 1].prev = NULL;
    pool->pool[capacity - 1].loc = NULL;
    pool->pool[capacity - 1].size = 0;
    pool->pool[capacity - 1].is_free = 0;

    return pool;
}

MemoryNode *get_node_from_pool(NodePool *pool)
{
    if (pool == NULL || pool->free_list == NULL)
        return NULL;

    MemoryNode *node = pool->free_list;
    pool->free_list = node->next;
    pool->used++;

    node->next = NULL;
    node->prev = NULL;
    node->loc = NULL;
    node->size = 0;
    node->is_free = 0;

    return node;
}

void return_node_to_pool(NodePool *pool, MemoryNode *node)
{
    if (pool == NULL || node == NULL)
        return;

    node->next = pool->free_list;
    node->prev = NULL;
    node->loc = NULL;
    node->size = 0;
    node->is_free = 0;

    if (pool->free_list != NULL)
        pool->free_list->prev = node;

    pool->free_list = node;
    pool->used--;
}

void cleanup_node_pool(NodePool *pool)
{
    if (pool == NULL)
        return;

    if (pool->pool != NULL)
        munmap(pool->pool, pool->capacity * sizeof(MemoryNode));
    munmap(pool, sizeof(NodePool));
}