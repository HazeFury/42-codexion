/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 11:20:15 by marberge          #+#    #+#             */
/*   Updated: 2026/05/27 11:20:25 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Initializes the global mutexes of the simulation.
** Returns 1 on success, 0 on failure.
*/
static int	init_global_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->sim_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->write_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->sim_mutex);
		return (0);
	}
	return (1);
}

/*
** Initializes each dongle, its physical mutex and its condition variable.
*/
static int	init_dongles(t_sim *sim)
{
	size_t	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		sim->dongles[i].id = i;
		sim->dongles[i].available_at = 0;
		sim->dongles[i].wait_queue = NULL;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			return (0);
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

/*
** Initializes coders, assigns their dongles and initializes their state mutex.
** Uses modulo to link the last coder to the first dongle.
*/
static int	init_coders(t_sim *sim)
{
	size_t	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].sim = sim;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1)
			% sim->args.number_of_coders];
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
			return (0);
		i++;
	}
	return (1);
}

/*
** Main initialization function. Allocates memory for arrays
** and calls sub-initialization functions.
*/
int	init_simulation(t_sim *sim)
{
	sim->start_time = 0;
	sim->is_running = 1;
	sim->dongles = malloc(sizeof(t_dongle) * sim->args.number_of_coders);
	if (!sim->dongles)
		return (0);
	sim->coders = malloc(sizeof(t_coder) * sim->args.number_of_coders);
	if (!sim->coders)
	{
		free(sim->dongles);
		return (0);
	}
	if (!init_global_mutexes(sim) || !init_dongles(sim) || !init_coders(sim))
		return (0);
	return (1);
}
