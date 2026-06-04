/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:40:54 by marberge          #+#    #+#             */
/*   Updated: 2026/06/04 14:22:36 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_sim	sim;

	memset(&sim, 0, sizeof(t_sim));
	if (!parse_arguments(argc, argv, &sim.args))
		return (1);
	if (!init_simulation(&sim))
	{
		printf("Error: Initialization failed.\n");
		return (1);
	}
	if (!launch_simulation(&sim))
	{
		printf("Error: Thread creation failed.\n");
		free_simulation(&sim);
		return (1);
	}
	free_simulation(&sim);
	return (0);
}
