/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: myakoven <myakoven@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/18 20:25:45 by spitul            #+#    #+#             */
/*   Updated: 2024/11/08 14:08:10 by myakoven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./include/minishell.h"

/*checks if here is a builtin command in a pipefree tree*/
int	builtin_check_walk(t_cmd *cmd)
{
	struct s_execcmd	*ecmd;
	struct s_redircmd	*rcmd;
	int					res;

	ecmd = NULL;
	rcmd = NULL;
	res = 0;
	if (cmd)
	{
		if (cmd->type == EXEC)
		{
			ecmd = (t_execcmd *)cmd;
			if (!ecmd->argv[0])
				return (0);
			res = is_builtin(ecmd->argv[0]);
			return (res);
		}
		else
		{
			rcmd = (t_redircmd *)cmd;
			res = builtin_check_walk(rcmd->cmd);
			return (res);
		}
	}
	return (res);
}

int	is_builtin(char *s)
{
	int	a;

	a = 0;
	if (ft_strncmp(s, ECHO, 5) == 0)
		a = 1;
	else if (ft_strncmp(s, CD, 3) == 0)
		a = 1;
	else if (ft_strncmp(s, PWD, 4) == 0)
		a = 1;
	else if (ft_strncmp(s, EXPORT, 7) == 0)
		a = 1;
	else if (ft_strncmp(s, UNSET, 6) == 0)
		a = 1;
	else if (ft_strncmp(s, ENV, 4) == 0)
		a = 1;
	else if (ft_strncmp(s, EXIT, 5) == 0)
		a = 1;
	return (a);
}

int	run_builtin(t_execcmd *cmd, t_tools *tool)
{
	int	a;

	a = 0;
	if (ft_strncmp(cmd->argv[0], ECHO, 5) == 0)
		a = echo(cmd);
	else if (ft_strncmp(cmd->argv[0], CD, 3) == 0)
		a = cd(cmd->argv, tool->env, tool);
	else if (ft_strncmp(cmd->argv[0], PWD, 4) == 0)
		a = pwd(cmd);
	else if (ft_strncmp(cmd->argv[0], EXPORT, 7) == 0)
		a = export(cmd, tool);
	else if (ft_strncmp(cmd->argv[0], UNSET, 6) == 0)
		a = unset(cmd, tool);
	else if (ft_strncmp(cmd->argv[0], ENV, 4) == 0)
		a = env(cmd->argv, tool->env, cmd, tool);
	// int	env(char **argv, char **env, t_execcmd *ecmd, t_tools *tools)
	else if (ft_strncmp(cmd->argv[0], EXIT, 5) == 0)
		a = ft_exit(cmd, tool);
	// tool->exit_code = a;
	record_exit(a, tool);
	return (a);
}

/*UNFINISHED*/
int	append_var(char *key, char *value, char **env, t_tools *tools)
{
	int		i;
	char	*newvar;

	newvar = NULL;
	i = 0;
	i = get_matrix_len(env);
	// while (env[i])
	// 	i++;
	if (i >= tools->env_len - 1)
		copy_env(tools, env); // adds MAXARGS
	newvar = ft_join_one(key, "=", value);
	if (!newvar)
		return (0);
	tools->env[i] = newvar;
	// has to be via tools in case of new env allocation
	return (1);
}

int	replace_or_append_var(char *key, char *value, char **env, t_tools *tools)
{
	int		i;
	char	*newvar;
	char	*temp;
	bool	found;

	found = 0;
	temp = NULL;
	i = 1;
	if (!key)
		return (0);
	while (env[i])
	{
		if (ft_strnstr(env[i], key, ft_strlen(key))
			&& env[i][ft_strlen(key)] == '=')
		{
			temp = env[i];
			break ;
		}
		i++;
	}
	if (temp)
	{
		found = true;
		newvar = ft_join_one(key, "=", value);
		if (!newvar)
			return (0);
		free(temp);
		env[i] = newvar;
	}
	if (!found)
		if (!append_var(key, value, env, tools))
			return (0);
	return (1);
}

int	replace_var(char *key, char *value, char **env)
{
	int		i;
	char	*temp;
	char	*newvar;

	i = 1;
	if (!key)
		return (0);
	while (env[i])
	{
		/*if we find the var value*/
		if (get_var_value(env, key))
		{
			temp = env[i];
			newvar = ft_join_one(key, "=", value);
			if (!newvar)
				return (0);
			free(temp);
			env[i] = newvar;
		}
		i++;
	}
	return (1);
}
