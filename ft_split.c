/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 06:12:22 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/21 06:12:24 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

char			**ft_split(const char *s, char c);
static size_t	get_word_count(const char *s, char sep);

char	**ft_split(const char *s, char c)
{
	char		**result;
	char		**r_temp;
	const char	*start = s + (s && *s == c);

	if (!s)
		return (NULL);
	result = (char **)malloc((get_word_count(s, c) + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	r_temp = result;
	while (*s++)
	{
		if (*s == c || !*s)
		{
			if (*(s - 1) != c)
			{
				*r_temp = ft_substr(start, 0, s - start);
				if (!*r_temp++)
					return (free_strs(result));
			}
			start = s + 1;
		}
	}
	*r_temp = NULL;
	return (result);
}

static size_t	get_word_count(const char *s, char sep)
{
	size_t	word_count;

	if (!s)
		return (0);
	word_count = 0;
	while (*s++)
		if ((*s == sep || !*s) && (*(s - 1) != sep))
			++word_count;
	return (word_count);
}
