/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tratanat <tawan.rtn@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/01 14:12:21 by tratanat          #+#    #+#             */
/*   Updated: 2025/06/21 19:10:15 by tratanat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include "global_mem.h"

void show_alloc_mem()
{
    size_t total = 0;

    MemoryNode *cur = blocks.tiny_head;
    ft_printf("TINY : 0x%X\n", cur);
    while (cur != NULL)
    {
        if (cur->is_free == 0)
        {
            ft_printf("0x%X - 0x%X : %u bytes\n", cur->loc,
                      cur->loc + cur->size, cur->size);
            total += cur->size;
        }
        cur = cur->next;
    }

    cur = blocks.small_head;
    ft_printf("SMALL : 0x%X\n", cur);
    while (cur != NULL)
    {
        if (cur->is_free == 0)
        {
            ft_printf("0x%X - 0x%X : %u bytes\n", cur->loc,
                      cur->loc + cur->size, cur->size);
            total += cur->size;
        }
        cur = cur->next;
    }

    cur = blocks.large_head;
    ft_printf("LARGE : 0x%X\n", cur);
    while (cur != NULL)
    {
        if (cur->is_free == 0)
        {
            ft_printf("0x%X - 0x%X : %u bytes\n", cur->loc,
                      cur->loc + cur->size, cur->size);
            total += cur->size;
        }
        cur = cur->next;
    }
    ft_printf("Total : %u bytes\n", total);
}