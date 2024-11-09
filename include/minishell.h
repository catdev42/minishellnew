/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: myakoven <myakoven@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/07 20:12:04 by myakoven          #+#    #+#             */
/*   Updated: 2024/11/09 22:03:16 by myakoven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdbool.h>
# include <stdio.h>
/* Keep on top */
# include <readline/history.h>
# include <readline/readline.h>
/* Keep on top */

# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
/**/
extern volatile sig_atomic_t global_signal; // TODO

# include "../libft/libft.h"
# include "builtins.h"
# include "structs.h"
/**/
# include "exits.h"

# define MIDLEN 256

# define UNEXP "syntax error near unexpected token "
# define UNCLOSED "unclosed quotes "

# define ECHO "echo"
# define CD "cd"
# define PWD "pwd"
# define EXPORT "export"
# define UNSET "unset"
# define ENV "env"
# define EXIT "exit"
/* error codes to send to error exit function error_exit_main(t_tools *tools,
		int error)*/
# define SYSTEMFAIL 142
# define FORKFAIL 141
# define UNKNOWNERROR 143

// volatile sig_atomic_t	global_signal = 0;

/*TESTERS*/
void			walking(struct s_cmd *cmd);

/*TEMP*/
void			free_things(char **s1, char **s2, char **s3, int fd);

/************************/
/******* BUILTINS.c ****/
/************************/
int				builtin_check_walk(t_cmd *cmd);

int				pwd(t_execcmd *cmd);

int				env(char **argv, char **env, t_execcmd *ecmd, t_tools *tools);
int				passcheck(char *start, long int lim);
char			*ft_join_one(char const *s1, char const *delim, char const *s2);

int				ft_exit(t_execcmd *cmd, t_tools *tool);

int				unset(t_execcmd *cmd, t_tools *tool);

int				export(t_execcmd *cmd, t_tools *tool);
// int				replace_var(char *key, char *value, char **env);
int				replace_or_append_var(char *key, char *value, char **env,
					t_tools *tools);

int				append_var(char *key, char *value, char **env, t_tools *tools);

int				echo(t_execcmd *cmd);

int				cd(char **argv, char **env, t_tools *tools);

/************************/
/*******  ENV.C  ********/
/************************/
char			*get_var(char **env, char *var);
int				get_var_i(char **env, char *var);
int				copy_env(t_tools *tools, char **env);
char			*get_var_value(char **env, char *var);

/************************/
/********* EXEC *********/
/************************/
/*exec_utils*/
void			check_system_fail(int status, t_tools *tools, bool inmain);
int				is_builtin(char *s);
int				run_builtin(t_execcmd *cmd, t_tools *tool);
int				file_dir_noexist(const char *path, int fd_in_or_out);
int				check_file_type(t_redircmd *rcmd, int fd_in_or_out);
/* exec */
void			handle_node(t_cmd *cmd, t_tools *tool);
void			run_exec_node(t_tools *tool, t_execcmd *ecmd);
void			execute_execve(char *pathcmd, t_execcmd *ecmd, t_tools *tool);
char			*check_cmd_path(char *path, t_execcmd *cmd, t_tools *tools);
int				running_msh(t_tools *tools);
int				run_pipeless_builtin_tree(t_cmd *cmd, t_tools *tool);
int				other_execution_type(t_tools *tool, t_execcmd *ecmd);
/* execredir */
int				run_redir(t_redircmd *rcmd, t_tools *tool);
void			run_pipe(t_pipecmd *pcmd, t_tools *tools);

/************************/
/******* SHELLinSHELL.C ********/
/************************/
int				ismini(char *cleanline, t_tools *tools);
void			change_shlvl(t_tools *tool);
void			exec_new_minishell(t_tools *tool, t_execcmd *ecmd);
int				fork_new_minishell(t_tools *tools);

/************************/
/******* INIT.C ********/
/************************/
struct s_cmd	*makeexec(void);
struct s_cmd	*makeredir(char *file, char *efile, bool append, int fd);
struct s_cmd	*makepipe(struct s_cmd *left, struct s_cmd *right);

/************************/
/******** LINE.C ********/
/************************/
int				copy_quotes(char *c_line, char *line, t_tools *tools);
int				copy_pipe(char *c_line, char *line, int current_line_index);
int				copy_redirect(char *c_line, char *line, int current_line_index);
char			*clean_line(char *line, int linelen, t_tools *tools);
int				copy_spaces(char *c_line, char *line);

/***** linesyntax.c *****/
int				valid_redirects(char *line);

/***** lineutils.c ******/
int				copy_var(char *c_line, char *line, t_tools *tools);
void			remove_useless_quotes(char *cline);
int				remove_two(char *first, char *second);

/************************/
/******** MAIN.C ********/
/************************/
int				shell_loop(t_tools *tools);
void			new_line(void);

/************************/
/****** SIGNALS.C *******/
/************************/
void			init_sa(struct sigaction *sa, void (*handler)(int));
void			handle_reprint_sig(int sig);
void			handle_printn_sig(int sig);
void			handle_recordonly_sig(int sig);
/************************/
/******* PARSE.C ********/
/************************/
struct s_cmd	*parseline(char *cline, t_tools *tools);
struct s_cmd	*createpipe(struct s_cmd *left, struct s_cmd *right,
					t_tools *tools);
char			*peek(char *line, char *end, int token);
/*static nullify*/
/******parse_heredoc.c*****/
void			here_init(char heredocs[MAXARGS][MAXARGS], t_tools *tools);
int				createredir_here(char *delim, int mode, int fd, t_tools *tools);
char			*make_heredoc_file(char *delim, t_tools *tools);
void			write_heredoc(int fd, char *alloc_delim, t_tools *tools);
char			*clean_line_expand_only(char *line, int linelen,
					t_tools *tools);

/***** parse_redir_exec.c ****/
struct s_cmd	*parseexec(char *start, char *end_of_exec, t_tools *tools);
struct s_cmd	*parse_redirs(char *start, char *end_of_exec, t_tools *tools);
int				createredir(char *filestart, int mode, int fd, t_tools *tools);
struct s_cmd	*parseargv(char *start, char *end, t_tools *tools);

/***** parse_utils.c *****/

int				infile_or_outfile(char *start);
char			*get_token_end(char *namestart);

/************************/
/******* utils.c ********/
/************************/
int				valid_quotes(char *line);

int				valid_line(char *line);
int				istoken(char c);
int				isquote(char c);
int				isredir(char c);

/******* utils2.c ********/
char			*safe_calloc(size_t nmemb, size_t size, t_tools *tools);
int				record_exit(int exit_num, t_tools *tools);
void			init_zero(size_t *i, size_t *j, char **str1, char **str2);
/******* UTILS3.C *******/
int				print_tab(char **envp);
int				get_matrix_len(char **matrix);
int				check_quotes(char *line, int i);
int				skip_quotes(char *line, int i);
int				skip_token(char *start, int i);

#endif

// int				pwd(t_execcmd *cmd);
// int				ft_exit(t_execcmd *cmd, t_tools *tool);
// // int				env(t_execcmd *cmd, t_tools *tool);
// int				unset(t_execcmd *cmd, t_tools *tool);
// int				export(t_execcmd *cmd, t_tools *tool);
// // int				cd(t_execcmd *cmd, t_tools *tool);
// int				echo(t_execcmd *cmd);
// int				builtin_check_walk(t_cmd *cmd);

// int				cd(char **argv, char **env, t_tools *tools);
// char			*ft_join_one(char const *s1, char const *delim, char const *s2);
// int				replace_var(char *key, char *value, char **env);
// int				passcheck(char *start, long int lim);