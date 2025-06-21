/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:18:00 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 22:52:38 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FT_MALLOC__
#define __FT_MALLOC__

#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include "../libft/libft.h"

#define NODE_POOL_SIZE 1000

typedef struct MemoryNode
{
    struct MemoryNode *next;
    struct MemoryNode *prev;
    void *loc;
    size_t size;
    int is_free;
} MemoryNode;

typedef struct NodePool
{
    MemoryNode *pool;
    MemoryNode *free_list;
    size_t used;
    size_t capacity;
} NodePool;

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
    NodePool *node_pool;
} MemoryBlocks;

void show_alloc_mem();
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *allocate_zone(size_t size);

NodePool *init_node_pool(size_t capacity);
MemoryNode *get_node_from_pool(NodePool *pool);
void return_node_to_pool(NodePool *pool, MemoryNode *node);
void cleanup_node_pool(NodePool *pool);

#endif