/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:13:08 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 18:59:23 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "global_mem.h"
#include "mem_blocks.h"

void free(void *ptr)
{
    if (ptr == NULL)
        return;
    pthread_mutex_lock(&lock);
    MemoryNode *block = get_block(ptr);
    if (block == NULL)
        return;
    free_block(block);
    pthread_mutex_unlock(&lock);
}
