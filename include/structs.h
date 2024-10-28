/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: myakoven <myakoven@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 20:00:21 by myakoven          #+#    #+#             */
/*   Updated: 2024/10/28 14:51:40 by myakoven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H
extern volatile sig_atomic_t global_signal; // TODO

# include "./minishell.h"

# define MAXARGS 20
/* Type of node */
# define EXEC 1
# define REDIR 2
# define PIPE 3

struct						s_cmd
{
	int						type;
};

struct						s_execcmd
{
	int						type;
	char					*argv[MAXARGS];
	char					*eargv[MAXARGS];
};

struct						s_redircmd
{
	int						type;

	bool					append;
	// 0xt thing redir or exec
	struct s_cmd			*cmd;
	// pointer to filename
	char					*file;
	// pointer to end of the file name for nullify
	char					*efile;
	// 0 for read, 1 for write and truncate, 2 for append?
	int						mode;
	// fd of the opened file?
	int						fd;
};

struct						s_pipecmd
{
	int						type;
	// execute first
	struct s_cmd			*left;
	// pipe into - this can also become the next pipe's left side
	struct s_cmd			*right;
};

typedef struct s_execcmd	t_execcmd;
typedef struct s_pipecmd	t_pipecmd;
typedef struct s_redircmd	t_redircmd;
typedef struct s_cmd		t_cmd;

typedef struct s_tools
{
	struct sigaction *sa; // its not an error

	char					**env;
	int						env_len;

	char					*line;
	size_t					line_capacity;

	char					*cleanline;
	char					*e_cline;
	size_t					cl_capacity;

	int						exit_code;
	char					*exit_string;

	struct s_cmd			*tree;

	/* Temporary */
	char					*s;
	char					*cmd_end;
	char					heredocs[MAXARGS][MAXARGS];
	int						hereindex;
	struct s_pipecmd 		*lastpipe;   // just set to null
	struct s_redircmd 		*lastredir;  // just set to null
}							t_tools;

#endif

/*
// //one for each command with its own redirects and argc
// typedef struct s_parsed_command
// {
// 	char		*line;
// 	int			argc;
// 	char		**argv;
// 	char		**redirects;
// 	t_token		*redir_tokens;
// }				t_parsed;
// typedef struct line_saver
// {
// 	char	**line_pointers;
// 	int		*line_lengths;
// }

// enum				e_tokentype
// {
// 	NONE,
// 	PIPE,
// 	INPUT,
// 	HEREDOC,
// 	OUTFILE,
// 	OUTFILE_APPEND,
// };
*/