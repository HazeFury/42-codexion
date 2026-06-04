/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/28 13:57:22 by marberge          #+#    #+#             */
/*   Updated: 2026/06/04 11:30:20 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Returns the current time in milliseconds.
** Converts seconds and microseconds from gettimeofday.
*/
long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/*
** Safely prints the state of a coder, ensuring no interleaved outputs.
** Only prints if the simulation is still running.
*/
void	print_state(t_coder *coder, const char *msg)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->sim->write_mutex);
	pthread_mutex_lock(&coder->sim->sim_mutex);
	if (coder->sim->is_running)
	{
		timestamp = get_time_in_ms() - coder->sim->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, msg);
	}
	pthread_mutex_unlock(&coder->sim->sim_mutex);
	pthread_mutex_unlock(&coder->sim->write_mutex);
}

/*
** Sleeps for the specified amount of milliseconds in small chunks.
** Breaks early if the simulation stops (e.g., due to a burnout).
*/
void	smart_sleep(long long time_to_sleep, t_sim *sim)
{
	long long	start;
	int			is_running;

	start = get_time_in_ms();
	while ((get_time_in_ms() - start) < time_to_sleep)
	{
		pthread_mutex_lock(&sim->sim_mutex);
		is_running = sim->is_running;
		pthread_mutex_unlock(&sim->sim_mutex);
		if (!is_running)
			break ;
		usleep(500);
	}
}
