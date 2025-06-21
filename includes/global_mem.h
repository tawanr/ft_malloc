/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global_mem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:34:17 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 22:34:30 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __GLOBAL_MEM__
#define __GLOBAL_MEM__

#include "malloc.h"

extern MemoryBlocks blocks;
extern pthread_mutex_t lock;
extern NodePool *node_pool;

#endif