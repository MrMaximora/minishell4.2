/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proces.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: derey <derey@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 08:47:13 by derey             #+#    #+#             */
/*   Updated: 2024/07/24 11:08:33 by derey            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	proces(t_input *line, int fd, t_init *init_shell, int o)
{
	int	i; 

	i = 0;
	/*while (line->all_tokens[i])
		i++;*/
	/*while (i > 0)
	{
		i--;
		if (line->all_tokens[i][0] == '|')
			break ;
	}*/
	if (i == 0 && o == 0)
		i = -1;
	else
		i = 0;
	if (ft_strnstr(line->all_tokens[i + 1], "cd", 3))
		cmd_cd(line, i + 2, init_shell);
	else if (ft_strnstr(line->all_tokens[i + 1], "echo", 4))
		cmd_echo(line->all_tokens, i + 2, fd, init_shell);
	else if (ft_strnstr(line->all_tokens[i + 1], "pwd", 3))
		cmd_pwd();
	else if (ft_strnstr(line->all_tokens[i + 1], "export", 6))
		cmd_export(line->all_tokens, i + 2, init_shell);
	else if (ft_strnstr(line->all_tokens[i + 1], "unset", 5))
		cmd_unset(line->all_tokens, i + 2, init_shell);
	else if (ft_strnstr(line->all_tokens[i + 1], "env", 3))
		cmd_env(init_shell, fd);
	else if (ft_strnstr(line->all_tokens[i + 1], "exit", 4))
		cmd_exit(line, i + 2, init_shell);
}

int	check_process(t_lexer_tab **tab_lexer, t_execute *process, t_init *init)
{
	t_lexer_tab	*curent;

	curent = *tab_lexer;
	while (curent)
	{
		if (curent->type == TOKEN_PIPE)
			process->n_pipe++;
		if (curent->type != TOKEN_ARGS && curent->type != TOKEN_SEMICOLON
			&& curent->next == NULL && curent->type
				!= TOKEN_BUILTIN && curent->type != TOKEN_EXPAND)
		{
			ft_printf_error(" syntax error near unexpected token `%s'\n", curent->arg);
			init->exit_code = 2;
			return (1);
		}
		else if (curent->index == 0 && curent->type == TOKEN_PIPE)
		{
			ft_printf_error(" syntax error near unexpected token `%s'\n", curent->arg);
			init->exit_code = 2;
			return (1);
		}
		curent = curent->next;
	}
	return (0);
}
