/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_input.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 23:12:47 by seonhwan          #+#    #+#             */
/*   Updated: 2024/11/21 02:53:12 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

char				**tokenize_input(t_shell_info *shell);
static t_quote_node	*parse_quotes(t_shell_info *shell);
static int		get_quote_segments_list(\
		char *str, t_quote_node *node, int *str_idx, int *seg_start);
static t_quote_node	*get_unquotes_tokens(t_shell_info *shell);
static t_quote_node	*process_dollar(t_shell_info *shell);
static t_quote_node	*process_tilde(t_shell_info *shell);
static char			*replace_env_vars(t_shell_info *shell, char *s, int compress_space_flag);
static char			*compress_space(char *s);
static int			get_env_key_len(char *env_start);
static char			*replace_str(char **str, const char *new_str, int start, int len);
static char			**convert_quote_list_to_tokens(t_quote_node *quote_list);
t_quote_node		*del_quote_list(t_quote_node **quote_list);

char	**tokenize_input(t_shell_info *shell)
{
	int	has_only_unquoted_whitespace;
	t_quote_node	*node;

	if (!shell)
		return (NULL);
	del_quote_list(&shell->quote_list);
	shell->quote_list = (t_quote_node *)ft_calloc(1, sizeof(t_quote_node));
	if (!shell->quote_list || !parse_quotes(shell))
		return (NULL);
	has_only_unquoted_whitespace = \
			(!shell->quote_list->quote && \
			!shell->quote_list->next && \
			!search_first_non_whitespace(shell->quote_list->raw_str));
	if (has_only_unquoted_whitespace || \
		!get_unquotes_tokens(shell))
	{
		printf("aa \n");
		return (NULL);
	}
	
	node = shell->quote_list;
	while (node)
	{
		printf("raw_str %s \n", node->raw_str);
		node = node->next;
	}

	if (!process_dollar(shell) || !process_tilde(shell))
		return (NULL);
	return (convert_quote_list_to_tokens(shell->quote_list));
}

static t_quote_node	*parse_quotes(t_shell_info *shell)
{
	t_quote_node	*node;
	int				i;
	int				start;

	node = shell->quote_list;
	i = -1;
	start = 0;
	if (isquote(shell->user_input[0]))
	{
		node->quote = shell->user_input[0];
		++i;
		++start;
	}
	if (get_quote_segments_list(shell->user_input, node, &i, &start) == -1)
		return (NULL);
	return (shell->quote_list);
}

/*
	1. 0 ->''
	2. 0->""

	3. ''->''
	4. ''->""
	6. ""->''
	7. ""->""

	5. ''->0
	8. ""->0
*/
static int	get_quote_segments_list(\
	char *str, t_quote_node *node, int *str_idx, int *seg_start)
{
	t_quote_node	*prev_node;
	char			has_unquoted_next;

	while (str[++*str_idx])
	{
		if ((!node->quote && isquote(str[*str_idx])) || \
			(node->quote && str[*str_idx] == node->quote))
		{
			node->raw_str = ft_substr(str, *seg_start, *str_idx - *seg_start);
			if (!node->raw_str)
				return (-1);
			prev_node = node;
			if ((!node->quote && !ft_isspace(str[*str_idx - 1])) || \
				(node->quote && !ft_isspace(str[*str_idx + 1]) && str[*str_idx + 1]))
				node->link_to_next = 1;
			has_unquoted_next = (node->quote && !isquote(str[*str_idx + 1]));
			if (node->quote && isquote(str[*str_idx + 1]))
				++*str_idx;
			*seg_start = *str_idx + 1;
			if (has_unquoted_next && !str[*seg_start])
				break ;
			node = (t_quote_node *)ft_calloc(1, sizeof(t_quote_node));
			if (!node)
				return (-1);
			prev_node->next = node;
			if (!has_unquoted_next)
				node->quote = str[*str_idx];
		}
	}
	if (!node->raw_str && node->quote)
		return (report_custom_err(\
					0, "custom error", "the quote is not closed.\n", -1));
	else if (*str_idx > *seg_start)
	{
 		node->raw_str = ft_substr(str, *seg_start, *str_idx - *seg_start);
		if (!node->raw_str)
			return (-1);
	}
	return (0);
}

static t_quote_node	*get_unquotes_tokens(t_shell_info *shell)
{
	t_quote_node	*node;

	if (!shell)
		return (NULL);
	node = shell->quote_list;
	while (node)
	{
		if (!node->quote)
		{
			node->tokens = split_with_metachars(node->raw_str, ' ');
			if (!node->tokens)
				return (NULL);
		}
		node = node->next;
	}
	return (shell->quote_list);
}

static t_quote_node	*process_dollar(t_shell_info *shell)
{
	t_quote_node	*node;
	int				i;
	char			*temp;

	node = shell->quote_list;
	while (node)
	{
		if (!node->quote)
		{
			i = -1;
			while (node->tokens[++i])
			{
				temp = node->tokens[i];
				node->tokens[i] = replace_env_vars(shell, node->tokens[i], 1);
				free(temp);
				if (!node->tokens[i])
					return (NULL);
			}
		}
		else if (node->quote == '\"')
		{
			temp = node->raw_str;
			node->raw_str = replace_env_vars(shell, node->raw_str, 0);
			free(temp);
			if (!node->raw_str)
				return (NULL);
		}
		node = node->next;
	}
	return (shell->quote_list);
}

static t_quote_node	*process_tilde(t_shell_info *shell)
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
}

static char	*replace_env_vars(t_shell_info *shell, char *s, int compress_space_flag)
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
		if (compress_space_flag)
			env_pair[1] = compress_space(get_env_value(shell->env_list, env_pair[0]));
		else
			env_pair[1] = ft_strdup(get_env_value(shell->env_list, env_pair[0]));
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

static char    *compress_space(char *s)
{
    char    **splitted_str;
    char    *result;

    splitted_str = ft_split(s, ' ');
    result = strs_join(splitted_str, " ");
    free_strs(splitted_str);
    return (result);
}

// if (s[i] == '$' && 앞 토큰이 "<<")
// {
// 	if (!s[i + 1] && node->next && !ft_strchr(METACHARS, node->next->raw_str[0]))
// 		$ 제거;
// 	else
// 		$ 보존;
// }

static int	get_env_key_len(char *env_start)
{
	char	*env_end;

	if (!env_start)
		return (-1);
	else if (ft_strchr(METACHARS, *env_start))
		return (0);
	else if (!ft_isalpha(*env_start) && *env_start != '_')
		return (-1);
	env_end = env_start;
	while (ft_isalnum(*++env_end) || *env_end == '_')
		;
	return (env_end - env_start);
}

static char	*replace_str(char **str, const char *new_str, int start, int len)
{
	char	*s;
	int		i;
	char	*prefix;
	char	*postfix;

	if (!str || !*str)
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
	if (!new_str)
		new_str = "";
	*str = strs_join((char *[]){prefix, (char *)new_str, postfix, NULL}, NULL);
	free(prefix);
	free(postfix);
	return (*str);
}

static char	**convert_quote_list_to_tokens(t_quote_node *quote_list)
{
	int				token_num;
	t_quote_node	*node;
	char			**tokens;
	int				i;
	int				j;
	char			*linked_token;

	if (!quote_list)
		return (NULL);
	token_num = 0;
	node = quote_list;
	while (node)
	{
		if (!node->quote)
			token_num += get_strs_num(node->tokens) - node->link_to_next;
		else if (!node->link_to_next)
			++token_num;
		printf("node->link_to_next : %d \n", node->link_to_next);
		node = node->next;
	}
	printf("token_num : %d \n", token_num);
	tokens = (char **)malloc((token_num + 1) * sizeof(char *));
	if (!tokens)
		return (NULL);
	linked_token = NULL;
	node = quote_list;
	i = -1;
	while (node)
	{
		if (!node->quote)
		{
			j = 0;
			if (node->tokens[j] && !node->link_to_next)
			{
				tokens[++i] = ft_strjoin(linked_token, node->tokens[j++]);
				if (!tokens[i])
				{
					free(linked_token);
					return (free_strs(tokens));
				}
			}
			while (node->tokens[j])
			{
				if (!node->tokens[j + 1] && node->link_to_next)
				{
					if (!strjoin_inplace(&linked_token, node->tokens[j++]))
					{
						free(linked_token);
						return (free_strs(tokens));
					}
					continue ;
				}
				tokens[++i] = ft_strdup(node->tokens[j++]);
				if (!tokens[i])
					return (NULL);
			}
		}
		else
		{
			if (node->link_to_next)
			{
				if (!strjoin_inplace(&linked_token, node->raw_str))
				{
					free(linked_token);
					return (free_strs(tokens));
				}
			}
			else
			{
				tokens[++i] = ft_strjoin(linked_token, node->raw_str);
				// tokens[++i] = strprepend_inplace(linked_token, &node->raw_str);
				if (!tokens[i])
					return (NULL);
			}
		}
		if (!node->link_to_next)
		{
			free(linked_token);
			linked_token = NULL;
		}
		node = node->next;
	}
	tokens[++i] = NULL;
	printf("i : %d \n", i);
	return (tokens);
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
