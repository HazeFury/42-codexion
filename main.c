/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:40:54 by marberge          #+#    #+#             */
/*   Updated: 2026/05/28 11:20:49 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*
** Helper function to print a coder's info based on the policy.
*/
static void	print_coder_info(t_coder *coder, t_scheduler policy)
{
	long long	deadline;

	if (policy == SCHEDULER_FIFO)
	{
		printf("Coder ID: %d | Request time: %lld\n",
			coder->id, coder->request_time);
	}
	else
	{
		deadline = coder->last_compile_start
			+ coder->sim->args.time_to_burnout;
		printf("Coder ID: %d | Deadline: %lld\n", coder->id, deadline);
	}
}

/*
** Tests the FIFO scheduling policy.
*/
static void	test_fifo(t_coder *coders)
{
	t_heap	heap;
	t_coder	*popped;

	printf("========== TESTING FIFO SCHEDULER ==========\n");
	init_heap(&heap, 10);
	coders[0].request_time = 500;
	coders[1].request_time = 100;
	coders[2].request_time = 300;
	coders[3].request_time = 800;
	coders[4].request_time = 200;
	heap_push(&heap, &coders[0], SCHEDULER_FIFO);
	heap_push(&heap, &coders[1], SCHEDULER_FIFO);
	heap_push(&heap, &coders[2], SCHEDULER_FIFO);
	heap_push(&heap, &coders[3], SCHEDULER_FIFO);
	heap_push(&heap, &coders[4], SCHEDULER_FIFO);
	printf("Expected order of IDs: 2, 5, 3, 1, 4\n");
	while (heap.size > 0)
	{
		popped = heap_pop(&heap, SCHEDULER_FIFO);
		print_coder_info(popped, SCHEDULER_FIFO);
	}
	free_heap(&heap);
}

/*
** Tests the EDF scheduling policy including the tie-breaker.
*/
static void	test_edf(t_coder *coders)
{
	t_heap	heap;
	t_coder	*popped;

	printf("\n========== TESTING EDF SCHEDULER ==========\n");
	init_heap(&heap, 10);
	coders[0].last_compile_start = 5000;
	coders[1].last_compile_start = 2000;
	coders[2].last_compile_start = 4000;
	coders[3].last_compile_start = 4000;
	coders[4].last_compile_start = 8000;
	heap_push(&heap, &coders[0], SCHEDULER_EDF);
	heap_push(&heap, &coders[1], SCHEDULER_EDF);
	heap_push(&heap, &coders[2], SCHEDULER_EDF);
	heap_push(&heap, &coders[3], SCHEDULER_EDF);
	heap_push(&heap, &coders[4], SCHEDULER_EDF);
	printf("Expected order of IDs: 2, 3, 4, 1, 5\n");
	while (heap.size > 0)
	{
		popped = heap_pop(&heap, SCHEDULER_EDF);
		print_coder_info(popped, SCHEDULER_EDF);
	}
	free_heap(&heap);
}

/*
** Temporary main to test the heap implementation.
*/
int	main(void)
{
	t_sim	sim;
	t_coder	coders[5];
	int		i;

	memset(&sim, 0, sizeof(t_sim));
	sim.args.time_to_burnout = 1000;
	i = 0;
	while (i < 5)
	{
		memset(&coders[i], 0, sizeof(t_coder));
		coders[i].id = i + 1;
		coders[i].sim = &sim;
		i++;
	}
	test_fifo(coders);
	test_edf(coders);
	return (0);
}
