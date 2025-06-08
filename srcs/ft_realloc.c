/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:14:15 by tratanat          #+#    #+#             */
/*   Updated: 2025/03/01 17:49:52 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include "global_mem.h"
#include "mem_blocks.h"

void *ft_realloc(void *ptr, size_t size) {
    pthread_mutex_lock(&lock);
    MemoryNode *block = get_block(ptr);
    if (block == NULL) {
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    MemoryNode *allocated = NULL;
    if (size > 0) {
        allocated = allocate_zone(size);
        if (allocated == NULL) {
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        ft_strlcpy(allocated->loc, block->loc, size);
    }
    free_block(block);
    pthread_mutex_unlock(&lock);
    if (allocated == NULL)
        return NULL;
    return allocated->loc;
}