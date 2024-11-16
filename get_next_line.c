/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 06:14:52 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/21 06:14:54 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <unistd.h>

char				*get_next_line(int fd, int *is_err, int del_node_flag);
static t_file_node	*add_file_node(\
		t_file_node **hash_tbl, int fd, t_file_node **node);
static char			*del_file_node(t_file_node **hash_tbl, t_file_node *node);
static char			*read_line(\
		int fd, char **lines, char *buf, size_t buf_capa);
static char			*get_line(char **lines, char **result, int *is_err);

char	*get_next_line(int fd, int *is_err, int del_node_flag)
{
	t_file_node			*node;
	static t_file_node	*hash_tbl[GNL_HASH_TABLE_SIZE];
	size_t				buf_capacity;
	char				*buf;
	char				*result;

	if (GNL_BUFFER_SIZE < 0 || fd < 0 || !is_err)
		return (NULL);
	if (!add_file_node(hash_tbl, fd, &node) && ++*is_err)
		return (NULL);
	buf_capacity = GNL_BUFFER_SIZE;
	if (buf_capacity && GNL_BUFFER_SIZE < GNL_CACHE_SIZE)
		buf_capacity *= GNL_CACHE_SIZE / buf_capacity;
	buf = (char *)malloc((buf_capacity + 1) * sizeof(char));
	if (!buf && ++*is_err)
		return (del_file_node(hash_tbl, node));
	result = read_line(fd, &node->str, buf, buf_capacity);
	free(buf);
	if (!result && ++*is_err)
		return (del_file_node(hash_tbl, node));
	if (!get_line(&node->str, &result, is_err))
		return (del_file_node(hash_tbl, node));
	if ((0 <= fd && fd <= 2) || del_node_flag)
		del_file_node(hash_tbl, node);
	return (result);
}

static t_file_node	*add_file_node(\
	t_file_node **hash_tbl, int fd, t_file_node **node)
{
	int		hash_value;

	hash_value = fd % GNL_HASH_TABLE_SIZE;
	*node = hash_tbl[hash_value];
	while (*node && (*node)->fd != fd)
		*node = (*node)->next;
	if (*node)
		return (*node);
	*node = (t_file_node *)malloc(sizeof(t_file_node));
	if (!*node)
		return (NULL);
	(*node)->fd = fd;
	(*node)->str = ft_substr("", 0, 0);
	if (!(*node)->str)
	{
		free(*node);
		return (NULL);
	}
	(*node)->next = hash_tbl[hash_value];
	hash_tbl[hash_value] = *node;
	return (*node);
}

static char	*del_file_node(t_file_node **hash_tbl, t_file_node *del_node)
{
	t_file_node	*prev;
	int			hash_value;

	hash_value = del_node->fd % GNL_HASH_TABLE_SIZE;
	if (!hash_tbl[hash_value])
		return (NULL);
	else if (del_node == hash_tbl[hash_value])
		hash_tbl[hash_value] = NULL;
	else
	{
		prev = hash_tbl[hash_value];
		while (prev->next && prev->next != del_node)
			prev = prev->next;
		if (!prev->next)
			return (NULL);
		prev->next = del_node->next;
	}
	free(del_node->str);
	free(del_node);
	return (NULL);
}

static char	*read_line(int fd, char **lines, char *buf, size_t buf_capa)
{
	size_t		cur_idx;
	ssize_t		nbytes;
	size_t		prev_idx;
	const int	cache_active = (buf_capa <= GNL_CACHE_SIZE);

	cur_idx = 0;
	nbytes = read(fd, buf, GNL_BUFFER_SIZE);
	while (nbytes > 0)
	{
		prev_idx = cur_idx;
		cur_idx = nbytes + (cur_idx * cache_active);
		buf[cur_idx] = '\0';
		cur_idx %= buf_capa;
		if (!cur_idx && !strjoin_inplace(lines, buf))
			return (NULL);
		if (ft_strchr(buf + (prev_idx * cache_active), '\n'))
			break ;
		nbytes = read(fd, buf + (cur_idx * cache_active), GNL_BUFFER_SIZE);
	}
	if (nbytes == -1 || (cur_idx && !strjoin_inplace(lines, buf)))
		return (NULL);
	return (*lines);
}

static char	*get_line(char **lines, char **result, int *is_err)
{
	char	*new_line_ptr;
	char	*temp;
	size_t	len;

	len = ft_strchr(*lines, '\0') - *lines;
	new_line_ptr = ft_strchr(*lines, '\n');
	if (!new_line_ptr)
		new_line_ptr = *lines + len;
	else
		++new_line_ptr;
	temp = ft_substr(new_line_ptr, 0, (*lines + len) - new_line_ptr);
	if (!temp && ++*is_err)
		return (NULL);
	*result = ft_substr(*lines, 0, new_line_ptr - *lines);
	if (!result && ++*is_err)
	{
		free(temp);
		return (NULL);
	}
	free(*lines);
	*lines = temp;
	return (*result);
}
