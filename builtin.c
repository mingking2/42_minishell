/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 20:52:34 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/16 20:52:43 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>

int			echo(t_shell_info *shell, char **args);
static char	**shift_option_n(char **args, int *has_option_n);
int			cd(t_shell_info *shell, char **args);
static int	change_directory(t_shell_info *shell, char *cd_arg);
static char	*get_cd_target_path(t_shell_info *shell, char *cd_arg);
int			pwd(t_shell_info *shell, char **args);
int			export(t_shell_info *shell, char **args);
int			unset(t_shell_info *shell, char **args);
int			env(t_shell_info *shell, char **args);
void		bash_exit(t_shell_info *shell, char **args);
static void	exit_shell(t_shell_info *shell, char **args, int exit_code);

int	echo(t_shell_info *shell, char **args)
{
	int	has_option_n;
	int	i;

	if (set_underscore_env(shell, "echo", args) == -1)
		return (-1);
	has_option_n = 0;
	args = shift_option_n(args, &has_option_n);
	if (args && *args)
	{
		i = 0;
		while (args[i + 1])
			printf("%s ", args[i++]);
		printf("%s", args[i]);
	}
	if (!has_option_n)
		printf("\n");
	return (0);
}

static char	**shift_option_n(char **args, int *has_option_n)
{
	int	i;
	int	j;

	if (!args || !has_option_n)
		return (NULL);
	i = 0;
	while (args[i] && args[i][0] == '-')
	{
		j = 1;
		while (args[i][j] == 'n')
			++j;
		if (args[i][j])
			break ;
		++i;
	}
	*has_option_n = (i != 0);
	return (args + i);
}

int	cd(t_shell_info *shell, char **args)
{
	char	*dir;

	if (set_underscore_env(shell, "cd", args) == -1)
		return (-1);
	if (!args || !args[0])
	{
		dir = get_env_value(shell->env_list, "HOME");
		if (!dir)
			return (report_custom_err(1, "cd", "HOME not set\n", -1));
	}
	else if (args[1])
		return (report_custom_err(1, "cd", "too many arguments\n", -1));
	else if (!ft_strcmp(args[0], "-"))
	{
		dir = get_env_value(shell->env_list, "OLDPWD");
		if (!dir)
			return (report_custom_err(1, "cd", "OLDPWD not set\n", -1));
		if (change_directory(shell, dir) == -1)
			return (-1);
		printf("%s\n", shell->cwd);
		return (0);
	}
	else
		dir = args[0];
	return (change_directory(shell, dir));
}

static int	change_directory(t_shell_info *shell, char *cd_arg)
{
	char	*err_source;
	char	*target_path;

	if (*cd_arg)
	{
		target_path = get_cd_target_path(shell, cd_arg);
		if (!target_path)
			return (-1);
		if (chdir(target_path) == -1)
		{
			free(target_path);
			err_source = ft_strjoin("cd: ", cd_arg);
			report_sys_err(1, err_source, -1);
			free(err_source);
			return (-1);
		}
		free(shell->cwd);
		shell->cwd = target_path;
	}
	if (replace_env_value(shell, "OLDPWD", \
			get_env_value(shell->env_list, "PWD"), 0) == -1 || \
		replace_env_value(shell, "PWD", shell->cwd, 0) == -1)
		return (-1);
	return (0);
}

static char	*get_cd_target_path(t_shell_info *shell, char *cd_arg)
{
	char	*target_path;
	char	*temp;
	int		double_slash_flag;

	if (*cd_arg == '/')
		target_path = ft_strdup(cd_arg);
	else if (!ft_strcmp(shell->cwd, "/") || !ft_strcmp(shell->cwd, "//"))
		target_path = strs_join((char *[]){shell->cwd, cd_arg, NULL}, NULL);
	else
		target_path = strs_join((char *[]){shell->cwd, cd_arg, NULL}, "/");
	double_slash_flag = (!ft_strncmp(target_path, "//", 2) && \
						target_path[2] != '/');
	temp = target_path;
	target_path = clean_path(target_path);
	free(temp);
	if (double_slash_flag && !strprepend_inplace("/", &target_path))
		return (free_and_return_null(target_path));
	return (target_path);
}

int	pwd(t_shell_info *shell, char **args)
{
	if (set_underscore_env(shell, "pwd", args) == -1)
		return (-1);
	printf("%s\n", shell->cwd);
	return (0);
}

int	export(t_shell_info *shell, char **args)
{
	t_env_list	*sorted_env_list;
	int			is_err;

	if (set_underscore_env(shell, "export", args) == -1)
		return (-1);
	if (!args || !args[0])
	{
		sorted_env_list = shell->sorted_env_list;
		while (sorted_env_list)
		{
			printf("declare -x %s", sorted_env_list->key);
			if (sorted_env_list->value)
				printf("=\"%s\"", sorted_env_list->value);
			printf("\n");
			sorted_env_list = sorted_env_list->next;
		}
		return (0);
	}
	is_err = 0;
	while (*args)
		if (update_env_list(shell, *args++) == -1)
			is_err = 1;
	return (-is_err);
}

int	unset(t_shell_info *shell, char **args)
{
	if (set_underscore_env(shell, "unset", args) == -1)
		return (-1);
	if (!args)
		return (0);
	while (*args)
	{
		if (!ft_strcmp(*args, "_"))
			continue ;
		del_env_list(extract_env(&shell->env_list, *args));
		del_env_list(extract_env(&shell->sorted_env_list, *args));
		++args;
	}
	return (0);
}

int	env(t_shell_info *shell, char **args)
{
	t_env_list	*env_node;

	if (set_underscore_env(shell, "env", args) == -1)
		return (-1);
	env_node = shell->env_list;
	while (env_node)
	{
		if (env_node->value)
			printf("%s=%s\n", env_node->key, env_node->value);
		env_node = env_node->next;
	}
	printf("_=/usr/bin/env\n");
	return (0);
}

void	bash_exit(t_shell_info *shell, char **args)
{
	int		is_err;
	int		exit_status;
	char	*err_source;

	if (!shell)
		return ;
	printf("exit\n");
	if (!args || !args[0])
		exit_shell(shell, args, shell->last_exit_status);
	is_err = 0;
	exit_status = strict_atoll(args[0], &is_err);
	if (is_err)
	{
		err_source = ft_strjoin("exit: ", args[0]);
		report_custom_err(1, err_source, "numeric argument required\n", -1);
		free(err_source);
		exit_shell(shell, args, 2);
	}
	if (!args[1])
		exit_shell(shell, args, exit_status);
	report_custom_err(1, "exit", "too many arguments\n", -1);
	exit_shell(shell, args, 1);
}

static void	exit_shell(t_shell_info *shell, char **args, int exit_code)
{
	free_strs(args);
	exit(clear_shell(shell, exit_code));
}
