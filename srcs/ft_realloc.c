/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:14:15 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 19:13:08 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "global_mem.h"
#include "mem_blocks.h"

void *realloc(void *ptr, size_t size)
{
    pthread_mutex_lock(&lock);
    // If ptr is NULL, should behave the same as malloc
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
        // Fix this
        size_t new_size = block->size;
        if (size < new_size)
            new_size = size;
        for (size_t i = 0; i < new_size; i++)
            *(char *)(allocated->loc + i) = *(char *)(block->loc + i);
        // ft_strlcpy(allocated->loc, block->loc, size);
    }
    free_block(block);
    pthread_mutex_unlock(&lock);
    if (allocated == NULL)
        return NULL;
    return allocated->loc;
}