/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 18:57:04 by marberge          #+#    #+#             */
/*   Updated: 2026/05/28 10:38:50 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Initializes the heap with a specific capacity.
** Returns 1 on success, 0 on memory allocation failure.
*/
int	init_heap(t_heap *heap, size_t capacity)
{
	heap->array = malloc(sizeof(t_coder *) * capacity);
	if (!heap->array)
		return (0);
	heap->size = 0;
	heap->capacity = capacity;
	return (1);
}

/*
** Frees the memory allocated for the heap array.
*/
void	free_heap(t_heap *heap)
{
	if (heap->array)
		free(heap->array);
	heap->array = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

/*
** Inserts a new coder into the priority queue.
** Returns 1 on success, 0 if the heap is full.
*/
int	heap_push(t_heap *heap, t_coder *coder, t_scheduler policy)
{
	if (heap->size >= heap->capacity)
		return (0);
	heap->array[heap->size] = coder;
	sift_up(heap, heap->size, policy);
	heap->size++;
	return (1);
}

/*
** Extracts and returns the most priority coder from the queue.
** Returns NULL if the heap is empty.
*/
t_coder	*heap_pop(t_heap *heap, t_scheduler policy)
{
	t_coder	*top;

	if (heap->size == 0)
		return (NULL);
	top = heap->array[0];
	heap->array[0] = heap->array[heap->size - 1];
	heap->size--;
	if (heap->size > 0)
		sift_down(heap, 0, policy);
	return (top);
}
