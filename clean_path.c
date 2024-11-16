/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_dot.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 18:30:16 by seonhwan          #+#    #+#             */
/*   Updated: 2024/11/08 18:30:17 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

char		*clean_path(char *abs_path);
static char	**path_split_and_count_non_dot(char *abs_path, int *non_dot_num);
static int	*mark_non_dot_indices(char **path_segments, int non_dot_num);
static void	process_double_dots(char **path_segments, int *non_dot_indices);
static char	**get_final_path_segments(\
	char **path_segments, int *non_dot_indices, int non_dot_num);

char	*clean_path(char *abs_path)
{
	int		non_dot_num;
	char	**path_segments;
	int		*non_dot_indices;
	char	**final_segments;
	char	*final_path;

	path_segments = path_split_and_count_non_dot(abs_path, &non_dot_num);
	if (!path_segments)
		return (NULL);
	else if (!non_dot_num && !free_strs(path_segments))
		return (ft_strdup("/"));
	non_dot_indices = mark_non_dot_indices(path_segments, non_dot_num);
	if (!non_dot_indices)
		return ((char *)free_strs(path_segments));
	final_segments = get_final_path_segments(\
						path_segments, non_dot_indices, non_dot_num);
	free_strs(path_segments);
	free(non_dot_indices);
	final_path = strs_join(final_segments, "/");
	free_strs(final_segments);
	if (final_path && !strprepend_inplace("/", &final_path))
		return (free_and_return_null(final_path));
	return (final_path);
}

static char	**path_split_and_count_non_dot(char *abs_path, int *non_dot_num)
{
	char	**path_segments;
	int		i;

	path_segments = ft_split(abs_path, '/');
	if (!path_segments)
		return (NULL);
	*non_dot_num = 0;
	i = -1;
	while (path_segments[++i])
		if (ft_strcmp(path_segments[i], ".") && \
			ft_strcmp(path_segments[i], ".."))
			++*non_dot_num;
	return (path_segments);
}

static int	*mark_non_dot_indices(char **path_segments, int non_dot_num)
{
	int	*non_dot_indices;
	int	i;
	int	j;

	non_dot_indices = (int *)malloc(non_dot_num * sizeof(int));
	if (!non_dot_indices)
		return (NULL);
	i = -1;
	j = -1;
	while (path_segments[++i])
		if (ft_strcmp(path_segments[i], ".") && \
			ft_strcmp(path_segments[i], ".."))
			non_dot_indices[++j] = i;
	return (non_dot_indices);
}

static char	**get_final_path_segments(\
	char **path_segments, int *non_dot_indices, int non_dot_num)
{
	int		result_segments_num;
	int		i;
	char	**result_segments;
	int		j;

	process_double_dots(path_segments, non_dot_indices);
	result_segments_num = 0;
	i = -1;
	while (++i < non_dot_num)
		if (non_dot_indices[i] != -1)
			++result_segments_num;
	result_segments = (char **)ft_calloc(\
								result_segments_num + 1, sizeof(char *));
	if (!result_segments)
		return (NULL);
	i = -1;
	j = -1;
	while (++i < non_dot_num)
		if (non_dot_indices[i] != -1 && \
			!strjoin_inplace(&result_segments[++j], \
						path_segments[non_dot_indices[i]]))
			break ;
	if (i < non_dot_num)
		return (free_strs(result_segments));
	return (result_segments);
}

static void	process_double_dots(char **path_segments, int *non_dot_indices)
{
	int	i;
	int	j;
	int	k;

	i = -1;
	j = -1;
	while (path_segments[++i])
	{
		if (!ft_strcmp(path_segments[i], "."))
			continue ;
		else if (!ft_strcmp(path_segments[i], ".."))
		{
			k = j;
			while (k > -1 && non_dot_indices[k] == -1)
				--k;
			if (k > -1)
				non_dot_indices[k] = -1;
			continue ;
		}
		++j;
	}
}
