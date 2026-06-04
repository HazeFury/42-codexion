/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 11:29:09 by marberge          #+#    #+#             */
/*   Updated: 2026/06/04 11:30:33 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Handles the specific case where only one coder is present.
** They take their only dongle and wait indefinitely for a second one.
*/
static void	handle_solo_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->mutex);
	print_state(coder, "has taken a dongle");
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	smart_sleep(coder->sim->args.time_to_burnout + 10, coder->sim);
}

/*
** Executes the compilation phase. Updates timestamps safely
** and uses smart_sleep to prevent blocking the simulation halt.
*/
static void	compile_phase(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_in_ms();
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->state_mutex);
	print_state(coder, "is compiling");
	smart_sleep(coder->sim->args.time_to_compile, coder->sim);
}

/*
** Helper to check if the simulation is still running safely.
*/
static int	is_sim_running(t_coder *coder)
{
	int	status;

	pthread_mutex_lock(&coder->sim->sim_mutex);
	status = coder->sim->is_running;
	pthread_mutex_unlock(&coder->sim->sim_mutex);
	return (status);
}

/*
** Main thread routine. Loops through the coder's lifecycle.
** Uses an initial delay for even IDs to reduce lock contention.
*/
void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->sim->args.number_of_coders == 1)
	{
		handle_solo_coder(coder);
		return (NULL);
	}
	if (coder->id % 2 == 0)
		smart_sleep(10, coder->sim);
	while (is_sim_running(coder))
	{
		coder->request_time = get_time_in_ms();
		take_dongles(coder);
		if (!is_sim_running(coder))
			break ;
		compile_phase(coder);
		drop_dongles(coder);
		print_state(coder, "is debugging");
		smart_sleep(coder->sim->args.time_to_debug, coder->sim);
		print_state(coder, "is refactoring");
		smart_sleep(coder->sim->args.time_to_refactor, coder->sim);
	}
	return (NULL);
}
