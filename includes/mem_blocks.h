/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_blocks.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:34:22 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 19:10:35 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __MEM_BLOCKS__
#define __MEM_BLOCKS__

#include "malloc.h"

MemoryNode *allocate_new_block(MemoryNode **head, size_t block_size);
void get_block_limit(size_t size, MemoryNode ***head, size_t *resolution, size_t *block_size);
MemoryNode *get_block(void *loc);
int free_block(MemoryNode *block);

#endif