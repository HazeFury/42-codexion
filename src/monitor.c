#include "codexion.h"

/*
** Checks if a single coder has burned out.
** Returns 1 if burned out, 0 otherwise.
*/
static int	check_burnout(t_coder *coder)
{
	long long	last_compile;
	long long	current_time;

	pthread_mutex_lock(&coder->state_mutex);
	last_compile = coder->last_compile_start;
	pthread_mutex_unlock(&coder->state_mutex);
	current_time = get_time_in_ms();
	if ((current_time - last_compile) >= coder->sim->args.time_to_burnout)
	{
		pthread_mutex_lock(&coder->sim->write_mutex);
		pthread_mutex_lock(&coder->sim->sim_mutex);
		if (coder->sim->is_running)
		{
			printf("%lld %d burned out\n",
				current_time - coder->sim->start_time, coder->id);
			coder->sim->is_running = 0;
		}
		pthread_mutex_unlock(&coder->sim->sim_mutex);
		pthread_mutex_unlock(&coder->sim->write_mutex);
		return (1);
	}
	return (0);
}

/*
** Checks if all coders have reached the required number of compiles.
** Returns 1 if all are done, 0 otherwise.
*/
static int	check_all_compiled(t_sim *sim)
{
	size_t	i;
	size_t	finished_count;
	size_t	compiles;

	i = 0;
	finished_count = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		compiles = sim->coders[i].compiles_done;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (compiles >= sim->args.number_of_compiles_required)
			finished_count++;
		i++;
	}
	if (finished_count == sim->args.number_of_coders)
	{
		pthread_mutex_lock(&sim->sim_mutex);
		sim->is_running = 0;
		pthread_mutex_unlock(&sim->sim_mutex);
		return (1);
	}
	return (0);
}

/*
** The monitor thread routine. Continuously checks for burnouts
** or if all coders have completed their required compiles.
*/
void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	size_t	i;

	sim = (t_sim *)arg;
	while (1)
	{
		i = 0;
		while (i < sim->args.number_of_coders)
		{
			if (check_burnout(&sim->coders[i]))
				return (NULL);
			i++;
		}
		if (check_all_compiled(sim))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
