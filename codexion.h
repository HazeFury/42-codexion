/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 11:40:51 by marberge          #+#    #+#             */
/*   Updated: 2026/05/26 15:52:34 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}				t_scheduler;

typedef struct s_args
{
	size_t			number_of_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	size_t			number_of_compiles_required;
	long long		dongle_cooldown;
	t_scheduler		scheduler;
}				t_args;

// ================================  PARSING  ================================

int	parse_arguments(int argc, char **argv, t_args *args);

#endif