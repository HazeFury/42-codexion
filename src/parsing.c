/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 13:35:01 by marberge          #+#    #+#             */
/*   Updated: 2026/05/26 15:44:08 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Checks if a string contains strictly digits.
 * Returns 1 if valid, 0 otherwise.
 */
static int	is_strictly_digits(const char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

/**
 * Converts a string to a positive long long integer.
 * Returns -1 if the string contains non-digit characters.
 */
static long long	parse_number(const char *str)
{
	long long	result;
	int			i;

	result = 0;
	i = 0;
	if (!is_strictly_digits(str))
		return (-1);
	while (str[i])
	{
		result = (result * 10) + (str[i] - '0');
		i++;
	}
	return (result);
}

// Map the scheduler string argument to the enum
static int	parse_scheduler(const char *str, t_scheduler *scheduler)
{
	if (strcmp(str, "fifo") == 0)
	{
		*scheduler = SCHEDULER_FIFO;
		return (1);
	}
	if (strcmp(str, "edf") == 0)
	{
		*scheduler = SCHEDULER_EDF;
		return (1);
	}
	return (0);
}

// Validate that no parsing returned -1 and coders > 0
static int	check_parsed_vals(t_args *args)
{
	if (args->number_of_coders == (size_t)-1 || args->time_to_burnout == -1
		|| args->time_to_compile == -1 || args->time_to_debug == -1
		|| args->time_to_refactor == -1 || args->dongle_cooldown == -1
		|| args->number_of_compiles_required == (size_t)-1
		|| args->number_of_coders == 0)
	{
		return (0);
	}
	return (1);
}

// Main parsing function to be called from main.c
int	parse_arguments(int argc, char **argv, t_args *args)
{
	if (argc != 9)
		return (0);
	args->number_of_coders = parse_number(argv[1]);
	args->time_to_burnout = parse_number(argv[2]);
	args->time_to_compile = parse_number(argv[3]);
	args->time_to_debug = parse_number(argv[4]);
	args->time_to_refactor = parse_number(argv[5]);
	args->number_of_compiles_required = parse_number(argv[6]);
	args->dongle_cooldown = parse_number(argv[7]);
	if (!check_parsed_vals(args) || !parse_scheduler(argv[8], &args->scheduler))
	{
		printf("Wrong value detected during parsing. Aborting programm !\n");
		return (0);
	}
	return (1);
}
