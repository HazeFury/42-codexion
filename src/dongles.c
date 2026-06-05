/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 11:28:49 by marberge          #+#    #+#             */
/*   Updated: 2026/06/05 11:52:32 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Lazy initialization of the dongle's wait queue.
** Called only the first time a dongle is requested.
** Returns 1 on success, 0 on malloc failure.
*/
static int	init_d_heap(t_coder *coder, t_dongle *dongle)
{
	dongle->wait_queue = malloc(sizeof(t_heap));
	if (!dongle->wait_queue)
		return (0);
	if (!init_heap((t_heap *)dongle->wait_queue,
			coder->sim->args.number_of_coders))
	{
		free(dongle->wait_queue);
		dongle->wait_queue = NULL;
		return (0);
	}
	return (1);
}

/*
** Helper to gracefully abort the simulation if a malloc fails dynamically.
*/
static void	handle_heap_fail(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&coder->sim->sim_mutex);
	coder->sim->is_running = 0;
	pthread_mutex_unlock(&coder->sim->sim_mutex);
	pthread_mutex_unlock(&dongle->mutex);
}

/*
** Adds the coder to the dongle's queue and waits until it is available.
** The dongle is available if available_at != -1 AND cooldown is over
** AND the coder is at the top of the heap.
*/
static void	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_heap	*heap;

	pthread_mutex_lock(&dongle->mutex);
	if (!dongle->wait_queue && !init_d_heap(coder, dongle))
	{
		handle_heap_fail(coder, dongle);
		return ;
	}
	heap = (t_heap *)dongle->wait_queue;
	heap_push(heap, coder, coder->sim->args.scheduler);
	while (is_sim_running(coder))
	{
		if (dongle->available_at != -1 && heap->array[0] == coder)
			if (get_time_in_ms() >= dongle->available_at)
				break ;
		pthread_mutex_unlock(&dongle->mutex);
		usleep(500);
		pthread_mutex_lock(&dongle->mutex);
	}
	if (is_sim_running(coder))
	{
		dongle->available_at = -1;
		heap_pop(heap, coder->sim->args.scheduler);
	}
	pthread_mutex_unlock(&dongle->mutex);
}

/*
** Takes both dongles. To prevent deadlocks, always locks the dongle
** with the lowest ID first.
*/
void	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->left_dongle;
	second = coder->right_dongle;
	if (first->id > second->id)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	acquire_dongle(coder, first);
	if (!is_sim_running(coder))
		return ;
	print_state(coder, "has taken a dongle");
	acquire_dongle(coder, second);
	if (is_sim_running(coder))
		print_state(coder, "has taken a dongle");
}

/*
** Releases both dongles and sets their availability timestamp
** to respect the cooldown parameter.
*/
void	drop_dongles(t_coder *coder)
{
	long long	cooldown;

	cooldown = coder->sim->args.dongle_cooldown;
	pthread_mutex_lock(&coder->left_dongle->mutex);
	coder->left_dongle->available_at = get_time_in_ms() + cooldown;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	coder->right_dongle->available_at = get_time_in_ms() + cooldown;
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}
