/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launcher.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 14:16:56 by marberge          #+#    #+#             */
/*   Updated: 2026/06/04 17:42:45 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Waits for all threads (monitor and coders) to finish their execution.
** This prevents stack memory leaks when aborting midway.
*/
static void	join_threads(t_sim *sim, size_t count, pthread_t *mon, int mon_ok)
{
	size_t	i;

	if (mon_ok)
		pthread_join(*mon, NULL);
	i = 0;
	while (i < count)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

/*
** Creates all the coder threads and tracks how many were successful.
** Returns 1 on success, 0 if any thread creation fails.
*/
static int	start_coders(t_sim *sim, size_t *created)
{
	*created = 0;
	while (*created < sim->args.number_of_coders)
	{
		sim->coders[*created].last_compile_start = sim->start_time;
		if (pthread_create(&sim->coders[*created].thread, NULL,
				coder_routine, &sim->coders[*created]) != 0)
			return (0);
		(*created)++;
	}
	return (1);
}

/*
** Main simulation launcher with rollback mechanism to prevent
** thread stack leaks if a pthread_create fails dynamically.
*/
int	launch_simulation(t_sim *sim)
{
	pthread_t	monitor;
	size_t		created;

	sim->start_time = get_time_in_ms();
	if (sim->start_time == -1)
		return (0);
	if (!start_coders(sim, &created))
	{
		pthread_mutex_lock(&sim->sim_mutex);
		sim->is_running = 0;
		pthread_mutex_unlock(&sim->sim_mutex);
		join_threads(sim, created, &monitor, 0);
		return (0);
	}
	if (pthread_create(&monitor, NULL, monitor_routine, sim) != 0)
	{
		pthread_mutex_lock(&sim->sim_mutex);
		sim->is_running = 0;
		pthread_mutex_unlock(&sim->sim_mutex);
		join_threads(sim, created, &monitor, 0);
		return (0);
	}
	join_threads(sim, created, &monitor, 1);
	return (1);
}
