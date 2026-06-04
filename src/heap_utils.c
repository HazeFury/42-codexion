/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 18:57:49 by marberge          #+#    #+#             */
/*   Updated: 2026/06/04 11:29:24 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Compares two coders based on the scheduling policy.
** Returns 1 if c1 is strictly more priority than c2, else 0.
*/
int	is_more_vip(t_coder *c1, t_coder *c2, t_scheduler policy)
{
	long long	d1;
	long long	d2;

	if (policy == SCHEDULER_FIFO)
		return (c1->request_time < c2->request_time);
	d1 = c1->last_compile_start + c1->sim->args.time_to_burnout;
	d2 = c2->last_compile_start + c2->sim->args.time_to_burnout;
	if (d1 == d2)
		return (c1->id < c2->id);
	return (d1 < d2);
}

/*
** Swaps two coder pointers in the heap array.
*/
void	swap_coders(t_coder **a, t_coder **b)
{
	t_coder	*tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
** Shifts up the element at index idx to restore the min-heap property.
** Used during insertion (push).
*/
void	sift_up(t_heap *heap, size_t idx, t_scheduler policy)
{
	size_t	parent;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (is_more_vip(heap->array[idx], heap->array[parent], policy))
		{
			swap_coders(&heap->array[idx], &heap->array[parent]);
			idx = parent;
		}
		else
			break ;
	}
}

/*
** Shifts down the element at index idx to restore the min-heap property.
** Used during extraction (pop).
*/
void	sift_down(t_heap *heap, size_t idx, t_scheduler policy)
{
	size_t	left;
	size_t	right;
	size_t	best;

	while (1)
	{
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		best = idx;
		if (left < heap->size && is_more_vip(heap->array[left],
				heap->array[best], policy))
			best = left;
		if (right < heap->size && is_more_vip(heap->array[right],
				heap->array[best], policy))
			best = right;
		if (best != idx)
		{
			swap_coders(&heap->array[idx], &heap->array[best]);
			idx = best;
		}
		else
			break ;
	}
}
