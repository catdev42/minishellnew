/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_heredoc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: myakoven <myakoven@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 19:16:34 by myakoven          #+#    #+#             */
/*   Updated: 2024/10/28 14:01:54 by myakoven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/minishell.h"

// ==250131== 40 bytes in 1 blocks are definitely lost in loss record 14 of 65
// ==250131==    at 0x4848899: malloc (in
// /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==250131==    by 0x403AE3: makeredir (init.c:30)
// ==250131==    by 0x405395: createredir_here (parse_heredoc.c:81)
// ==250131==    by 0x4057F5: parse_redirs (parse_redir_exec.c:54)
// ==250131==    by 0x4056A8: parseexec (parse_redir_exec.c:22)
// ==250131==    by 0x405E5F: parseline (parse.c:52)
// ==250131==    by 0x405102: shell_loop (main.c:72)
// ==250131==    by 0x404F42: main (main.c:31)

// rewrite nullify to use actual information

/*
if I press control c... I have to leave heredoc function and return to minishell
i need to create the file and record the name while in main process and launch ONLY the heredoc readline in a separate process
*/

// tools->lastredir;
// launchreadlineloop
// write what it sees into a file and turn it into a regular infile
int	createredir_here(char *delim, int append, int fd, t_tools *tools)
{
	char	*end;
	char	*filename;
	int		len;

	end = get_token_end(delim);
	filename = make_heredoc_file(delim, tools);
	if (!filename)
		return (0);
	len = end - delim;
	if (tools->lastredir)
	{
		tools->lastredir->cmd = makeredir(filename, end, append, fd);
		tools->lastredir = (struct s_redircmd *)tools->lastredir->cmd;
	}
	else
		tools->lastredir = (struct s_redircmd *)makeredir(filename, end, append,
				fd);
	if (!tools->lastredir)
		error_exit_main(tools, 1);
	return (len);
}

/*Gives user the cursor - must check*/
char	*make_heredoc_file(char *delim, t_tools *tools)
{
	char	*end;
	char	*tempalloc_delim;
	int		fd;
	pid_t	pid;

	pid = -1;
	init_zero(NULL, NULL, &end, &tempalloc_delim);
	end = get_token_end(delim);
	fd = open(tools->heredocs[tools->hereindex++],
			O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
		error_exit_main( tools, 1); // exits the program and cleansfiles
	tempalloc_delim = ft_substr(delim, 0, end - delim);
	if (!tempalloc_delim)
		error_exit_main( tools, 1); 
	pid = fork();
	if (pid == -1)
		error_exit_main( tools, 1); 
	if (pid == 0)
		write_heredoc(fd, tempalloc_delim, tools); // put in fork!
	waitpid(pid, &tools->exit_code, 0);
	if (global_signal == SIGINT)
		return (NULL);
	check_system_fail(tools->exit_code, tools, 1); // we are in main
	close(fd);
	return (tools->heredocs[tools->hereindex - 1]);
}

/*Gives user the cursor - must check*/
void	write_heredoc(int fd, char *alloc_delim, t_tools *tools)
{
	char	*line;

	while (1)
	{
		line = NULL;
		init_sa(tools->sa, handle_here_sig); // passing a function
		line = readline("heredoc: ");
		if (global_signal == SIGINT)
			good_exit(tools);
		init_sa(tools->sa, SIG_DFL); // passing a function
		if (!line || ft_strncmp(line, alloc_delim, ft_strlen(alloc_delim)) == 0)
		{
			if (!line)
				print_error("warning", "here-doc delimited by EOF, wanted ",
					alloc_delim);
			break ;
		}
		if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1)
		{
			free(line);
			free(alloc_delim);
			print_errno_exit(NULL, NULL, errno, tools); // we are in fork
		}
		free(line);
	}
	free(line);
	free(alloc_delim);
	close(fd);
	good_exit(tools); // exits for 0 for success while cleaning
}

/* Initialize the heredoc names struct */
void	here_init(char heredocs[MAXARGS][MAXARGS], t_tools *tools)
{
	int		i;
	char	*tempalloc;

	i = 0;
	while (i < MAXARGS)
	{
		tempalloc = NULL;
		tempalloc = ft_itoa(i);
		if (!tempalloc)
			error_exit_main(tools, errno);
		ft_strlcpy(heredocs[i], "heredoc", MAXARGS);
		ft_strlcat(heredocs[i], tempalloc, MAXARGS);
		free(tempalloc);
		i++;
	}
	return ;
}

