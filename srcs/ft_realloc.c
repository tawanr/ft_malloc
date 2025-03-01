/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:14:15 by tratanat          #+#    #+#             */
/*   Updated: 2025/03/01 14:28:28 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "mem_blocks.h"
#include "global_mem.h"

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
        if (new_block == NULL || new_block == MAP_FAILED)
            return NULL;
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