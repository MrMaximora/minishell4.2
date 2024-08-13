/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: derey <derey@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 10:51:11 by maxouvra          #+#    #+#             */
/*   Updated: 2024/07/25 14:16:41 by derey            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	path_early(t_init *init_shell)
{
	char	c[PATH_MAX];
	int		i;
	int		x;

	getcwd(c, sizeof(c));
	i = find_homes(init_shell);
	free(init_shell->pathearly);
	x = (int)ft_strlen(c);
	if (i <= x)
	{
		if (i < x)
			init_shell->pathearly = ft_calloc(x - i + 5, sizeof(char *));
		else
			init_shell->pathearly = ft_calloc(5, sizeof(char *));
		if (init_shell->pathearly == NULL)
			return ;
		x = 0;
		init_shell->pathearly[x++] = '~';
		while (i <= (int)ft_strlen(c) && c[i] != '\0')
			init_shell->pathearly[x++] = c[i++];
		init_shell->pathearly[x++] = '$';
		init_shell->pathearly[x++] = ' ';
	}
	else
		init_shell->pathearly = ft_strjoin(c, "$ ");
}

int	ft_exp_dir(t_init *init_shell, char *src)
{
	int		i;
	int		x;
	char	*tmp;

	i = 1;
	x = 0;
	tmp = ft_calloc(ft_strlen(src), sizeof(char));
	while (src[i] != '\0' && src[i] != ' ')
	{
		tmp[x] = src[i];
		i++;
		x++;
	}
	i = 0;
	while (init_shell->cpy_env[i] != NULL)
	{
		if (ft_find_export(init_shell->cpy_env[i], tmp, '='))
			return (1);
		i++;
	}
	return (0);
}

int	acces_dirnocd(char *result, t_init *init_shell)
{
	if (access(result, F_OK) == -1)
	{
		free(result);
		ft_putstr_fd(" No such file or directory\n", 2);
		init_shell->exit_code = 127;
		return (0);
	}
	if (access(result, F_OK | R_OK) != 0)
	{
		free(result);
		ft_putstr_fd(" Permission denied\n", 2);
		init_shell->exit_code = 126;
		return (0);
	}
	return (1);
}

void	ft_dir_nocd(t_init *init_shell, char *src, int x, int i)
{
	char	c[PATH_MAX];
	char	*result;
	char	*tmp;
	char	*join;
	char	*oldpwd;

	tmp = ft_calloc(ft_strlen(src), sizeof(char));
	if (getcwd(c, sizeof(c)) == NULL)
		return ;
	oldpwd = c;
	while (src[i] != '\0')
		tmp[x++] = src[i++];
	join = ft_strjoin(c, "/");
	result = ft_strjoin(join, tmp);
	free(tmp);
	free(join);
	acces_dirnocd(result, init_shell);
	cd_end(oldpwd, result, init_shell);
	ft_putstr_fd(" Is a directory\n", 2);
	init_shell->exit_code = 126;
}

void	ft_dir_base(t_init *init_shell, char *src)
{
	if (access(src, F_OK) == -1)
	{
		ft_putstr_fd(" No such file or directory\n", 2);
		init_shell->exit_code = 127;
		return ;
	}
	if (access(src, F_OK | R_OK) != 0)
	{
		ft_putstr_fd(" Permission denied\n", 2);
		init_shell->exit_code = 126;
		return ;
	}
	ft_putstr_fd(" Is a directory\n", 2);
	init_shell->exit_code = 126;
}

void	loop_main(t_input line, t_execute *process, t_init *init_shell)
{
	int	i;
	t_lexer_tab	*tab_lexer;

	add_history(line.input);
	ft_expand(&line, init_shell);
	if (!line.all_tokens)
	{
		ft_putstr_fd("malloc error\n", 2);
		return ;
	}
	i = 0;
	tab_lexer = NULL;
	while (line.all_tokens[i])
	{
		tab_lexer = create_lexer_tab(tab_lexer, &line, i);
		i++;
	}
	if (check_process(&tab_lexer, process, init_shell) == 1)
	{
		ft_free_next_line(tab_lexer, &line);
		init_shell->exit_code = 2;
		return ;
	}
	if (line.input[0] == '$')
	{
		if (line.input[1] != '?' && line.input[1] != '\0')
		{
			if (ft_exp_dir(init_shell, line.input) == 1
				&& line.all_tokens[1] == NULL)
			{
				//proces(&line, 1, init_shell, 0);
				//ft_putstr_fd(" Is a directory\n", 2);
				init_shell->exit_code = 126;
			}
			else if (line.all_tokens[1] != NULL)
				proces(&line, 1, init_shell, 1);
		}
		else
		{
			ft_putstr_fd(" command not found\n", 2);
			init_shell->exit_code = 127;
		}
	}
	else if (tab_lexer->type == TOKEN_BUILTIN && process->n_pipe == 0)
		proces(&line, 1, init_shell, 0);
	else if (line.input[0] == '.' && line.input[1] == '/')
		ft_dir_nocd(init_shell, line.all_tokens[0], 0, 2);
	else if (line.input[0] == '/')
		ft_dir_base(init_shell, line.all_tokens[0]);
	else if (process->n_pipe >= 0)
		pipeline_execute(&line, process, init_shell, tab_lexer);
	else
	{
		ft_putstr_fd(" command not found\n", 2);
		init_shell->exit_code = 127;
	}
	if (line.all_tokens)
		ft_free_next_line(tab_lexer, &line);
}

int	main(int ac, char **av, char **env)
{
	t_input		line;
	t_execute	*process;
	t_init		*init_shell;
	int			i;

	(void)av;
	(void)ac;
	init_shell = ft_calloc(1, sizeof(t_init));
	process = ft_calloc(1, sizeof(t_execute));
	ft_init(init_shell, process, env);
	sigaction(SIGQUIT, &init_shell->ignore, NULL);
	sigaction(SIGINT, &init_shell->sig, NULL);
	path_early(init_shell);
	line.input = readline(init_shell->pathearly);
	if (!line.input || !line.input[0])
		init_shell->exit_code = 0;
	else
		loop_main(line, process, init_shell);
	while (line.input && init_shell->on == 1)
	{
		line.input = readline(init_shell->pathearly);
		if (!line.input || !line.input[0])
			init_shell->exit_code = 0;
		else
			loop_main(line, process, init_shell);
	}
	clear_history();
	ft_free(init_shell->cpy_env);
	free(init_shell->pathearly);
	i = init_shell->exit_code;
	free(init_shell);
	free(process);
	exit(i);
}

char	*check_rights(char *cmd, char **env)
{
	char	*buff;
	char	**all_paths;
	char	*f_path;
	int		i;

	i = 0;
	if (access(cmd, X_OK) == 0)
		return (cmd);
	while (env[i] != NULL && ft_strnstr(env[i], "PATH", 4) == 0)
		i++;
	all_paths = ft_split(env[i] + 5, ':');
	i = 0;
	while (all_paths[i])
	{
		buff = ft_strjoin(all_paths[i], "/");
		f_path = ft_strjoin(buff, cmd);
		free(buff);
		if (access(f_path, F_OK | X_OK) == 0)
			return (f_path);
		free(f_path);
		i++;
	}
	ft_free_path(all_paths);
	return (NULL);
}

void	ft_free_path(char **all_paths)
{
	int	i;

	i = 0;
	while (all_paths[i])
		free(all_paths[i++]);
	free(all_paths);
}

void	pipe_init(t_execute *process, t_pipe *pipes)
{
	int			i;

	i = 0;
	process->multoutfile = 0;
	process->saved_fdout = -1;
	while (i <= process->n_pipe)
	{
		pipes[i].pid = -1;
		pipes[i].fdin = -1;
		pipes[i].fdout = -1;
		i++;
	}
}

void	set_pipe_fd(t_pipe *pipes, t_execute *process, int i)
{
	if (process->n_pipe == 0)
		return ;
	//ft_printf("pipes[i].fdout = %d\n", pipes[i].fdout);
	if (i == process->n_pipe && process->saved_fdout == -1)
	{
		pipes[i].fdout = STDOUT_FILENO;
		return ;
	}
	if (i != process->n_pipe && pipe(process->pipefd) == -1)
	{
		perror("pipe error");
		return ;
	}
	//ft_printf("i = %d, process->pipefd[0] = %d, process->pipefd[1] = %d\n", i, process->pipefd[0], process->pipefd[1]);
	if (pipes[i].fdout == -1)
	{
		if (process->saved_fdout != -1)
			pipes[i].fdout = process->saved_fdout;
		else
			pipes[i].fdout = process->pipefd[1];
		//ft_printf("pipes[%d].fdout = %d\n", i, pipes[i].fdout);
	}
	else if (pipes[i].fdout > -1)
		try_close(process->pipefd[1]);
	if (i >= 0)
	{
		pipes[i + 1].fdin = process->pipefd[0];
		//ft_printf("pipes[%d].fdin = %d\n", i + 1, pipes[i + 1].fdin);
	}

	//else {
        //printf("i = %d, non-pipe command\n", i);
    //}
}

int	try_close(int fd)
{
	if (fd > 2)
		return (close(fd));
	else
		return (-1);
}

int	set_file(t_execute *process, t_pipe *pipes, t_lexer_tab *current, int i)
{
		if (current->type == TOKEN_REDIRECT_INPUT)
		{
			pipes[i].fdin = open_file(current->next->arg, 0);
			if (pipes[i].fdin == -1)
				return (-1);
			//ft_printf("Opening file for input redirection: %s, fd: %d\n", current->next->arg, pipes[i].fdin);
			current->next->file_descriptor = 1;
		}
		if (current->type == TOKEN_REDIRECT_OUTPUT)
		{
			pipes[i].fdout = open_file(current->next->arg, 1);
			if (pipes[i].fdout == -1)
				return (-1);
			//ft_printf("Opening file for output redirection: %s, fd: %d\n", current->next->arg, pipes[i].fdout);
			process->multoutfile++;
			if (process->multoutfile > 1)
				close(process->saved_fdout);
			process->saved_fdout = pipes[i].fdout;
			current->next->file_descriptor = 1;
		}
		if (current->type == TOKEN_APPEND)
		{
			pipes[i].fdout = open_file(current->next->arg, 2);
			if (pipes[i].fdout == -1)
				return (-1);
			//ft_printf("Opening file for output append redirection: %s, fd: %d\n", current->next->arg, pipes[i].fdout);
			process->multoutfile++;
			if (process->multoutfile > 1)
				close(process->saved_fdout);
			process->saved_fdout = pipes[i].fdout;
			current->next->file_descriptor = 1;
		}
		return (0);
}
void	child_process(t_pipe *pipes, t_execute *process, int i)
{
	//printf("Child process %d: pipes[%d].fdin = %d, pipes[%d].fdout = %d\n", i, i, pipes[i].fdin, i, pipes[i].fdout);
	if (pipes[i].fdin != -1)
	{
		if (dup2(pipes[i].fdin, STDIN_FILENO) == -1)
		{
			perror("dup2 fdin error");
			exit(EXIT_FAILURE);
		}
		try_close(pipes[i].fdin);
	}
	if (pipes[i].fdout != -1 && pipes[i].fdout != STDOUT_FILENO)
	{
		//dprintf(2, "fd out : %d\n", pipes[i].fdout);
		if (dup2(pipes[i].fdout, STDOUT_FILENO) == -1)
		{
			perror("dup2 fdout error");
			exit(EXIT_FAILURE);
		}
		try_close(pipes[i].fdout);
	}
	if (process->saved_fdout != -1)
		try_close(process->saved_fdout);
	close_all_fds(pipes, process);
}

void	parent_process(t_pipe *pipes, t_execute *process)
{
	int i;
	
	i = 0;
	close_all_fds(pipes, process);
	while (i <= process->n_pipe)
	{
		waitpid(pipes[i].pid, NULL, 0);
		i++;
	}
}

void	close_all_fds(t_pipe *pipes, t_execute *process)
{
	int	i;

	i = 0;
	while (i <= process->n_pipe)
	{
		if (pipes[i].fdin != -1)
		{
			try_close(pipes[i].fdin);
			pipes[i].fdin = -1;
			//dprintf(2, "close all fds : fdin : %d\n", pipes[i].fdin);
		}
		if (pipes[i].fdout != -1)
		{
			try_close(pipes[i].fdout);
			pipes[i].fdout = -1;
			//dprintf(2, "close all fds : fdout : %d\n", pipes[i].fdout);
		}
		i++;
	}
}

void	pipeline_execute(t_input *line, t_execute *process, t_init *init_shell, t_lexer_tab *tab_lexer)
{
	t_lexer_tab	*current;
	int			i;
	char		**args;
	t_pipe		pipes[process->n_pipe + 1];

	current = tab_lexer;
	pipe_init(process, pipes);
	i = 0;
	while (current)
	{
		if (set_file(process, pipes, current, i) == -1)
		{
			ft_printf("%s: No such file or directory\n");
			init_shell->exit_code = 1;
			return ;
		}
		current = current->next;
	}
	i = 0;
	current = tab_lexer;
	while (i <= process->n_pipe && current)
	{
		if ((current->type == TOKEN_ARGS || current->type == TOKEN_BUILTIN) && current->file_descriptor == 0)
		{
			if (current->type == TOKEN_ARGS)
				args = get_command_args(line, &current);
			set_pipe_fd(pipes, process, i);
			pipes[i].pid = fork();
			if (pipes[i].pid == -1)
				perror("fork error");
			else if (pipes[i].pid == 0)
			{
				if (current->type == TOKEN_BUILTIN)
				{
					//ft_printf("process->pipefd[0] = %d\n", process->pipefd[0]);
					proces(line, process->pipefd[0], init_shell, current->index);
				}
				else
				{
					child_process(pipes, process, i);
					if (process->n_pipe != 0)
						try_close(process->pipefd[0]);
					exec_cmd(args, line, init_shell);
					free(args);	
				}
			}
			try_close(process->pipefd[1]);
			//ft_printf("i = %d\n", i);
			i++;
		}
		current = current->next;
	}
	parent_process(pipes, process);
	process->n_pipe = 0;
}

void	exec_cmd(char **args, t_input *line, t_init *init_shell)
{
	if (args[0] == NULL)
	{
		ft_printf("Invalid command or path not found\n");
		return ;
	}
	line->path = check_rights(args[0], init_shell->cpy_env);
	//ft_printf("PATH : %s\n", line->path);
	if (!line->path)
	{
		ft_printf_error(" command not found\n");
		init_shell->exit_code = 127;
		init_shell->on = 0;
	}
	else if (execve(line->path, args, init_shell->cpy_env) == -1)
	{
		perror("execve error");
	}
}

char	**get_command_args(t_input *line, t_lexer_tab **lexer_tab)
{
	char		**args;
	int			i;
	int			argc;
	int			start_index;
	t_lexer_tab *current;

	i = 0;
	argc = 0;
	current = *lexer_tab;
	start_index = current->index;
	while (current && line->all_tokens[current->index]
		&& strcmp(line->all_tokens[current->index], "|") != 0
		&& strcmp(line->all_tokens[current->index], "<") != 0
		&& strcmp(line->all_tokens[current->index], ">") != 0)
	{
		argc++;
		if (current->next)
			current = current->next;
		else
			break;
	}
	args = ft_calloc((argc + 1), sizeof(char *));
	while (i < argc)
	{
		args[i] = line->all_tokens[start_index + i];
		i++;
	}
	args[argc] = NULL;
	*lexer_tab = current;
	return (args);
}

int	open_file(char *file, int in_or_out)
{
	int	ret;

	if (in_or_out == 0)
		ret = open(file, O_RDONLY);
	if (in_or_out == 1)
		ret = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (in_or_out == 2)
		ret = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (ret == -1)
		ft_printf("%s: No such file or directory\n");
	return (ret);
}
