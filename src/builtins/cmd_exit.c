/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: derey <derey@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 09:09:49 by derey             #+#    #+#             */
/*   Updated: 2024/07/16 14:33:40 by derey            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	cmd_exit(t_input *line, int i, t_init *init_shell)
{
	init_shell->on = 0;
	if (!line->all_tokens[i])
		init_shell->exit_code = 0;
	else if (line->all_tokens[i + 1] != NULL)
	{
		init_shell->exit_code = 1;
		ft_putstr_fd(" too many arguments\n", 2);
	}
	else if (ft_isalpha(line->all_tokens[i][0]) == 1)
	{
		init_shell->exit_code = 2;
		ft_putstr_fd(" numeric argument required\n", 2);
	}
	else
		init_shell->exit_code = ft_atoi(line->all_tokens[i]);
	ft_printf("exit\n");
}
