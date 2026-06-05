/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 11:55:21 by marberge          #+#    #+#             */
/*   Updated: 2026/06/04 21:50:45 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Destroys all dongles' mutexes and condition variables, 
** then frees the allocated array.
*/
static void	free_dongles(t_sim *sim)
{
	size_t	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (sim->dongles[i].wait_queue)
		{
			free_heap((t_heap *)sim->dongles[i].wait_queue);
			free(sim->dongles[i].wait_queue);
		}
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}

/*
** Destroys all coders' state mutexes and frees the allocated array.
*/
static void	free_coders(t_sim *sim)
{
	size_t	i;

	if (!sim->coders)
		return ;
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
}

/*
** Main cleanup function to safely destroy all POSIX objects
** and free all dynamically allocated memory.
*/
void	free_simulation(t_sim *sim)
{
	if (!sim)
		return ;
	free_dongles(sim);
	free_coders(sim);
	pthread_mutex_destroy(&sim->sim_mutex);
	pthread_mutex_destroy(&sim->write_mutex);
}
