/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 23:12:47 by seonhwan          #+#    #+#             */
/*   Updated: 2024/11/10 00:32:10 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

t_quote_node	*parse_quotes(t_shell_info *shell);
t_quote_node	*del_quote_list(t_quote_node **quote_list);

t_quote_node	*parse_quotes(t_shell_info *shell)
{
	t_quote_node	*node;
	t_quote_node	*prev_node;
	int				i;
	int				start;

	del_quote_list(&shell->quote_list);
	shell->quote_list = (t_quote_node *)ft_calloc(1, sizeof(t_quote_node));
	if (!shell->quote_list)
		return (NULL);
	node = shell->quote_list;
	i = -1;
	start = 0;
	while (shell->user_input[++i])
	{
		if ((!node->quote && isquote(shell->user_input[i])) || \
			(node->quote && shell->user_input[i] == node->quote))
		{
			node->raw_str = ft_substr(shell->user_input, start, i - start);
			if (!node->raw_str)
				return (del_quote_list(&shell->quote_list));
			prev_node = node;
			node = (t_quote_node *)ft_calloc(1, sizeof(t_quote_node));
			if (!node)
				return (del_quote_list(&shell->quote_list));
			node->quote = shell->user_input[i];
			start = i + 1;
			prev_node->next = node;
		}
	}
	if (node->quote)
	{
		report_custom_err(0, "custom error", "the quote is not closed.\n", -1);
		return (del_quote_list(&shell->quote_list));
	}
	else if (start < i)
	{
		node->raw_str = ft_substr(shell->user_input, start, i - start);
		if (!node->raw_str)
			return (del_quote_list(&shell->quote_list));
	}
	return (shell->quote_list);
}

t_quote_node	*process_dollar(t_shell_info *shell)
{
	t_quote_node	*node;
	char			*temp;
	int				i;

	node = shell->quote_list;
	while (node)
	{
		if (!node->quote)
		{
			i = -1;
			while (node->tokens[++i])
			{
				;
			}
		}
		if (node->quote != '\'')
		{
			temp = node->raw_str;
			node->raw_str = replace_env_vars(shell, node->raw_str);
			free(temp);
			if (!node->raw_str)
				return (NULL);
		}
		node = node->next;
	}
	return (shell->quote_list);
}

t_quote_node	*process_tilde(t_shell_info *shell)
{
	t_quote_node	*node;
	int				i;

	node = shell->quote_list;
	while (node)
	{
		if (!node->quote)
		{
			if (!node->tokens)
				return (NULL);
			i = -1;
			while (node->tokens[++i])
			{
				if (node->tokens[i][0] == '~' && \
					(node->tokens[i][1] == '/'|| 
						(!node->tokens[i][1] && \
							(node->tokens[i + 1] || !node->next || \
							get_last_char(node->raw_str) != '~'))) && \
					!replace_str(&node->tokens[i], shell->homedir, 0, 1))
					return (NULL);
			}
		}
		node = node->next;
	}
	return (shell->quote_list);
}replace_str

char	*replace_env_vars(t_shell_info *shell, char *s)
{
	int		i;
	int		env_pair_len[2];
	char	*env_pair[2];
	char	*last_exit_status_str;
	int		is_err;

	if (!shell)
		return (NULL);
	s = ft_strdup(s);
	if (!s)
		return (NULL);
	i = -1;
	while (s[++i])
	{
		if (s[i] != '$')
			continue ;
		else if (s[i + 1] == '?')
		{
			last_exit_status_str = ft_itoa(shell->last_exit_status);
			is_err = !replace_str(&s, last_exit_status_str, i, 2);
			i += ft_strlen(last_exit_status_str) - 1;
			free(last_exit_status_str);
			if (is_err)
				return (free_and_return_null(s));
			continue ;
		}
		else if (!s[i + 1])
			break ;
		env_pair_len[0] = get_env_key_len(&s[i + 1]);
		if (env_pair_len[0] <= 0)
		{
			if (!env_pair_len[0] || s[i + 1])
				++i;
			continue ;
		}
		env_pair[0] = ft_substr(&s[i + 1], 0, env_pair_len[0]);
		env_pair[1] = get_env_value(shell->env_list, env_pair[0]);
		free(env_pair[0]);
		env_pair_len[1] = ft_strlen(env_pair[1]);
		is_err = !replace_str(&s, env_pair[1], i, env_pair_len[0] + 1);
		free(env_pair[1]);
		if (is_err)
			return (free_and_return_null(s));
		i += env_pair_len[1] - 1;
	}
	return (s);
}

// if (s[i] == '$' && 앞 토큰이 "<<")
// {
// 	if (!s[i + 1] && node->next && !ft_strchr(METACHARS, node->next->raw_str[0]))
// 		$ 제거;
// 	else
// 		$ 보존;
// }

int	get_env_key_len(char *env_start)
{
	char	*env_end;

	if (!env_start)
		return (-1);
	else if (ft_strchr(METACHARS, *env_start))
		return (0);
	else if (!ft_isalpha(*env_start) && !*env_start == '_')
		return (-1);
	env_end = env_start;
	while (ft_isalnum(*++env_end) || *env_end == '_')
		;
	return (env_end - env_start);
}

char	*replace_str(char **str, const char *new_str, int start, int len)
{
	char	*s;
	int		i;
	char	*prefix;
	char	*postfix;

	if (!str || !*str || !new_str)
		return (NULL);
	s = *str;
	while (*s && --start >= 0)
		++s;
	i = -1;
	while (s[++i] && i < len)
		;
	if (i < len)
		return (*str);
	prefix = ft_substr(*str, 0, s - *str);
	postfix = ft_strdup(s + len);
	if ((!prefix || !postfix) && \
		!free_and_return_null(prefix) && !free_and_return_null(postfix))
		return (NULL);
	free(*str);
	*str = strs_join((char *[]){prefix, new_str, postfix, NULL}, NULL);
	free(prefix);
	free(postfix);
	return (*str);
}

t_quote_node	*del_quote_list(t_quote_node **quote_list)
{
	t_quote_node	*node;
	t_quote_node	*next;

	if (!quote_list)
		return (NULL);
	node = *quote_list;
	while (node)
	{
		next = node->next;
		free(node->raw_str);
		free_strs(node->tokens);
		free(node);
		node = next;
	}
	*quote_list = NULL;
	return (NULL);
}
