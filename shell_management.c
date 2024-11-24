/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_management.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 02:37:07 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/17 02:37:41 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <limits.h>
#include <unistd.h>

int			init_shell(t_shell_info *shell, char *envp[]);
static char	*get_user_host_segment(char *envp[]);
int			clear_shell(t_shell_info *shell, int return_value);

int	init_shell(t_shell_info *shell, char *envp[])
{
	t_env_list	*env;
	char		**sorted_envp;
	char		*euid_str;
	const char	*admin_priv_segments[2] = {"$ ", "# "};

	if (!shell || !envp)
		return (-1);
	ft_memset(shell, 0, sizeof(t_shell_info));
	shell->initital_env = envp;
	shell->env_list = new_env_list(envp);
	if (!shell->env_list)
		return (-1);
	shell->underscore_env = extract_env(&shell->env_list, "_");
	env = shell->env_list;
	while (env && env->next)
		env = env->next;
	shell->last_env = env;
	sorted_envp = dup_strs(envp);
	if (selection_sort_strs(sorted_envp) == -1)
		return (clear_shell(shell, -1));
	shell->sorted_env_list = new_env_list(sorted_envp);
	free_strs(sorted_envp);
	if (!shell->sorted_env_list)
		return (clear_shell(shell, -1));
	del_env_list(extract_env(&shell->sorted_env_list, "_"));
	shell->last_exit_status = 0;
	shell->cwd = getcwd(NULL, PATH_MAX);
	if (!shell->cwd)
	{
		report_sys_err(1, "getcwd", -1);
		return (clear_shell(shell, -1));
	}
	shell->homedir = get_homedir(envp);
	shell->user_host_segment = get_user_host_segment(envp);
	if (!shell->homedir || !shell->user_host_segment)
		return (clear_shell(shell, -1));
	euid_str = get_euid_str(envp);
	if (!euid_str)
		return (clear_shell(shell, -1));
	shell->admin_priv_segment = admin_priv_segments[!ft_strcmp(euid_str, "0")];
	free(euid_str);
	shell->prompt = NULL;
	shell->user_input = NULL;
	shell->quote_list = NULL;
	return (0);
}

static char	*get_user_host_segment(char *envp[])
{
	char	*username;
	char	*hostname;
	char	*prompt_temp[2];
	int		is_err;
	char	*user_host_segment;

	if (!envp)
		return (NULL);
	username = get_username(envp);
	hostname = get_hostname(envp);
	prompt_temp[0] = ft_strjoin(username, "@");
	prompt_temp[1] = ft_strjoin(hostname, ":");
	is_err = !(username && hostname && prompt_temp[0] && prompt_temp[1]);
	free(username);
	free(hostname);
	if (!is_err)
		user_host_segment = ft_strjoin(prompt_temp[0], prompt_temp[1]);
	else
		user_host_segment = NULL;
	free(prompt_temp[0]);
	free(prompt_temp[1]);
	return (user_host_segment);
}

int	clear_shell(t_shell_info *shell, int return_value)
{
	if (!shell)
		return (return_value);
	del_env_list(shell->env_list);
	del_env_list(shell->underscore_env);
	del_env_list(shell->sorted_env_list);
	free(shell->cwd);
	free(shell->homedir);
	free(shell->user_host_segment);
	free(shell->prompt);
	free(shell->user_input);
	// del_quote_list(&shell->quote_list);
	//clear_history();
	return (return_value);
}
