/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launcher.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 14:16:56 by marberge          #+#    #+#             */
/*   Updated: 2026/06/04 14:16:58 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Waits for all threads (monitor and coders) to finish their execution.
** This prevents the main process from exiting prematurely.
*/
static void	join_threads(t_sim *sim, pthread_t monitor)
{
	size_t	i;

	pthread_join(monitor, NULL);
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

/*
** Creates all the coder threads and assigns them their routine.
** Returns 1 on success, 0 on thread creation failure.
*/
static int	start_coders(t_sim *sim)
{
	size_t	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
			return (0);
		i++;
	}
	return (1);
}

/*
** Main simulation launcher. Sets the starting time, creates
** the coder threads and the monitor thread, then waits for them.
** Returns 1 on success, 0 on failure.
*/
int	launch_simulation(t_sim *sim)
{
	pthread_t	monitor;

	sim->start_time = get_time_in_ms();
	if (sim->start_time == -1)
		return (0);
	if (!start_coders(sim))
		return (0);
	if (pthread_create(&monitor, NULL, monitor_routine, sim) != 0)
		return (0);
	join_threads(sim, monitor);
	return (1);
}
