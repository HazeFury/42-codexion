/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   toto.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marberge <marberge@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/18 11:11:33 by marberge          #+#    #+#             */
/*   Updated: 2026/05/18 11:59:32 by marberge         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

void	ft_swap(int *s1, int *s2)
{
	int	tmp;

	tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

char	*ft_strdup(char *str)
{
	int		i;
	char	*res;

	i = ft_strlen(str);
	res = malloc((i * sizeof(char) + 1));
	i = 0;
	while (str[i] != '\0')
	{
		res[i] = str[i];
		i++;
	}
	res[i] = '\0';
	return (res);
}

int	main(void)
{
	int		a;
	int		b;
	char	*test_str;

	a = 2;
	b = 4;
	printf("before : a = %d | b = %d\n", a, b);
	ft_swap(&a, &b);
	printf("after : a = %d | b = %d\n", a, b);
	printf("%d\n", ft_strlen("hello world"));
	test_str = ft_strdup("hello toto");
	printf("%s\n", test_str);
	free(test_str);
	return (0);
}
