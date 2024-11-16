/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 20:51:39 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/16 20:51:53 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

t_env_list			*new_env_list(char *envp[]);
static t_env_list	*alloc_parse_env(char *env_pair);
static t_env_list	*alloc_set_env(char *key, char *value);
t_env_list			*del_env_list(t_env_list *env);
int					update_env_list(t_shell_info *shell, char *env_pair);
int					is_valid_env_key(char *key);
char				*get_env_value(t_env_list *env, char *key);
int					replace_env_value(\
		t_shell_info *shell, char *key, char *value, int append_flag);
t_env_list			*search_env(t_env_list *env, char *key);
static int			add_env_list(t_shell_info *shell, t_env_list *env);
static t_env_list	*search_first_greater_key_env(\
		t_env_list *env, char *key, t_env_list **prev_node);
t_env_list			*extract_env(t_env_list **env_ptr_addr, char *key);
int					set_underscore_env(\
		t_shell_info *shell, char *cmd, char **args);

t_env_list	*new_env_list(char *envp[])
{
	t_env_list	*env_head;
	t_env_list	*env;
	t_env_list	*prev_env;

	if (!envp)
		return (NULL);
	env_head = alloc_parse_env(*envp);
	if (!env_head || !*envp)
		return (env_head);
	env = env_head;
	while (*++envp)
	{
		prev_env = env;
		env = alloc_parse_env(*envp);
		if (!env)
			return (del_env_list(env_head));
		prev_env->next = env;
	}
	return (env_head);
}

static t_env_list	*alloc_parse_env(char *env_pair)
{
	t_env_list	*env;
	char		*equal_sign_pos;
	int			add_flag;

	env = (t_env_list *)malloc(sizeof(t_env_list));
	if (!env)
		return (NULL);
	env->next = NULL;
	equal_sign_pos = ft_strchr(env_pair, '=');
	if (equal_sign_pos)
	{
		add_flag = (*(equal_sign_pos - 1) == '+');
		env->key = ft_substr(env_pair, 0, equal_sign_pos - add_flag - env_pair);
		env->value = ft_strdup(equal_sign_pos + 1);
		if (!env->key || !env->value)
			return (del_env_list(env));
	}
	else
	{
		env->key = ft_strdup(env_pair);
		env->value = NULL;
		if (!env->key)
			return (del_env_list(env));
	}
	return (env);
}

static t_env_list	*alloc_set_env(char *key, char *value)
{
	t_env_list	*env;

	env = (t_env_list *)malloc(sizeof(t_env_list));
	if (!env)
		return (NULL);
	env->next = NULL;
	env->key = ft_strdup(key);
	env->value = ft_strdup(value);
	if ((key && !env->key) || (value && !env->value))
		return (del_env_list(env));
	return (env);
}

t_env_list	*del_env_list(t_env_list *env)
{
	t_env_list	*next;

	while (env)
	{
		next = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = next;
	}
	return (NULL);
}

int	update_env_list(t_shell_info *shell, char *env_pair)
{
	t_env_list	*env;
	int			append_flag;

	if (!shell || !env_pair)
		return (-1);
	env = alloc_parse_env(env_pair);
	if (!env)
		return (-1);
	if (!is_valid_env_key(env->key))
	{
		report_export_key_err(env_pair, -1);
		return (-!del_env_list(env));
	}
	else if (get_env_value(shell->env_list, env->key) && !env->value)
		return (!!del_env_list(env));
	if (search_env(shell->env_list, env->key))
	{
		append_flag = (env->value && *(ft_strchr(env_pair, '=') - 1) == '+');
		if (replace_env_value(shell, env->key, env->value, append_flag) == -1)
			return (-!del_env_list(env));
		return (!!del_env_list(env));
	}
	if (add_env_list(shell, env) == -1)
		return (-!del_env_list(env));
	return (0);
}

int	is_valid_env_key(char *key)
{
	int	i;

	if (!key || (!ft_isalpha(key[0]) && key[0] != '_'))
		return (0);
	i = 1;
	while (ft_isalnum(key[i]) || key[i] == '_')
		++i;
	if (key[i])
		return (0);
	return (1);
}

char	*get_env_value(t_env_list *env, char *key)
{
	if (!env || !key)
		return (NULL);
	while (env && ft_strcmp(env->key, key))
		env = env->next;
	if (!env)
		return (NULL);
	return (env->value);
}

int	replace_env_value(\
	t_shell_info *shell, char *key, char *value, int append_flag)
{
	int					i;
	t_env_list *const	env_lists[2] = {\
							shell->env_list, shell->sorted_env_list};
	t_env_list			*env;

	if (!ft_strcmp(key, "_"))
		return (0);
	i = -1;
	while (++i < 2)
	{
		env = search_env(env_lists[i], key);
		if (!env)
			return (-1);
		if (append_flag && !strjoin_inplace(&env->value, value))
			return (-1);
		else if (!append_flag)
		{
			free(env->value);
			env->value = ft_strdup(value);
			if (!env->value)
				return (-1);
		}
	}
	return (0);
}

t_env_list	*search_env(t_env_list *env, char *key)
{
	if (!key)
		return (NULL);
	while (env)
	{
		if (!ft_strcmp(env->key, key))
			return (env);
		env = env->next;
	}
	return (NULL);
}

static int	add_env_list(t_shell_info *shell, t_env_list *env)
{
	t_env_list	*prev_env;

	if (!shell || !env)
		return (-1);
	if (!shell->env_list)
		shell->env_list = env;
	else
		shell->last_env->next = env;
	shell->last_env = env;
	env = alloc_set_env(env->key, env->value);
	if (!env)
		return (-1);
	env->next = search_first_greater_key_env(\
		shell->sorted_env_list, env->key, &prev_env);
	if (prev_env)
		prev_env->next = env;
	else
		shell->sorted_env_list = env;
	return (0);
}

static t_env_list	*search_first_greater_key_env(\
	t_env_list *env, char *key, t_env_list **prev_node)
{
	t_env_list	*prev_env;

	if (!key)
		return (NULL);
	prev_env = NULL;
	while (env)
	{
		if (ft_strcmp(env->key, key) > 0)
			break ;
		prev_env = env;
		env = env->next;
	}
	if (prev_node)
		*prev_node = prev_env;
	return (env);
}

t_env_list	*extract_env(t_env_list **env_ptr_addr, char *key)
{
	t_env_list	*env;
	t_env_list	*prev_env;

	if (!env_ptr_addr || !key)
		return (NULL);
	env = *env_ptr_addr;
	prev_env = NULL;
	while (env && ft_strcmp(env->key, key))
	{
		prev_env = env;
		env = env->next;
	}
	if (!env)
		return (NULL);
	if (!prev_env)
		*env_ptr_addr = env->next;
	else
		prev_env->next = env->next;
	env->next = NULL;
	return (env);
}

int	set_underscore_env(t_shell_info *shell, char *cmd, char **args)
{
	int	i;

	if (!shell || !cmd)
		return (-1);
	free(shell->underscore_env->value);
	if (!args)
	{
		shell->underscore_env->value = ft_strdup(cmd);
		if (!shell->underscore_env->value)
			return (-1);
		return (0);
	}
	i = -1;
	while (args[i + 1])
		++i;
	if (i == -1)
		shell->underscore_env->value = ft_strdup(cmd);
	else
		shell->underscore_env->value = ft_strdup(args[i]);
	if (!shell->underscore_env->value)
		return (-1);
	return (0);
}
