/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:14:15 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/23 14:05:43 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "global_mem.h"
#include "mem_blocks.h"

void *realloc(void *ptr, size_t size)
{
    pthread_mutex_lock(&lock);
    if (ptr == NULL)
    {
        pthread_mutex_unlock(&lock);
        return malloc(size);
    }
    MemoryNode *block = get_block(ptr);
    if (block == NULL)
    {
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    MemoryNode *allocated = NULL;
    if (size > 0)
    {
        allocated = allocate_zone(size);
        if (allocated == NULL)
        {
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        size_t new_size = block->size;
        if (size < new_size)
            new_size = size;
        ft_memcpy(allocated->loc, block->loc, new_size);
    }
    free_block(block);
    pthread_mutex_unlock(&lock);
    if (allocated == NULL)
        return NULL;
    return allocated->loc;
}