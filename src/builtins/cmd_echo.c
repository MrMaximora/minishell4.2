/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_echo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: derey <derey@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 09:08:22 by derey             #+#    #+#             */
/*   Updated: 2024/07/22 15:42:39 by derey            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	echo_n(char *str)
{
	int	i;

	i = 1;
	while (str[i] == 'n')
		i++;
	if (str[i] == '\0')
		return (1);
	else
		return (0);
}

static void	print_echo(char **list, int i, int fd, int n)
{
	int	q;

	q = 0;
	if (n == 1)
		i++;
	while (list[i] != NULL)
	{
		if (list[i][0] == '-' && echo_n(list[i]) == 1 && q == 0)
		{
			i++;
			continue ;
		}
		ft_putstr_fd(list[i], fd);
		if (list[i + 1] != NULL && list[i + 1][0] != ' ')
			ft_putstr_fd(" ", fd);
		q++;
		i++;
	}
}

void	cmd_echo(char **list, int i, int fd, t_init *init_shell)
{
	if (list[i] == NULL)
		ft_putstr_fd("\n", fd);
	else if (ft_strnstr(list[i], "-n", 2) && echo_n(list[i]) == 1)
		print_echo(list, i, fd, 1);
	else
	{
		print_echo(list, i, fd, 0);
		ft_putstr_fd("\n", fd);
	}
	init_shell->exit_code = 0;
}
