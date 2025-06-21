/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:18:00 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 19:13:58 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FT_MALLOC__
#define __FT_MALLOC__

#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include "../libft/libft.h"

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
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *allocate_zone(size_t size);

#endif