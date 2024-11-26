/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 04:15:04 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/22 04:15:05 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>

void	*ft_memset(void *s, int c, size_t n);
void	*ft_calloc(size_t nmemb, size_t size);
void	*free_and_return_null(void *ptr);
char	**free_strs(char **strs);

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	uc;
	size_t			mod_quo[2];
	unsigned char	*uc_ptr;
	size_t			bytes;
	size_t			*s_ptr;

	if (!s)
		return (NULL);
	uc = sizeof(size_t);
	mod_quo[0] = n % uc;
	mod_quo[1] = n / uc;
	if (mod_quo[1] == 1 && mod_quo[1]--)
		mod_quo[0] += 8;
	uc_ptr = (unsigned char *)s;
	while (mod_quo[0]-- > 0)
		*uc_ptr++ = (unsigned char)c;
	if (!mod_quo[1])
		return (s);
	bytes = (unsigned char)c;
	while (--uc > 0)
		bytes = (bytes << 8) | (unsigned char)c;
	s_ptr = (size_t *)uc_ptr;
	while (mod_quo[1]-- > 0)
		*s_ptr++ = bytes;
	return (s);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*result;

	if (nmemb && size && (((size_t)(-1) / size) < nmemb))
		return (NULL);
	result = malloc(nmemb * size);
	if (!result)
		return (NULL);
	ft_memset(result, 0, nmemb * size);
	return (result);
}

void	*free_and_return_null(void *ptr)
{
	free(ptr);
	return (NULL);
}

char	**free_strs(char **strs)
{
	char	**temp_strs;

	if (!strs)
		return (NULL);
	temp_strs = strs;
	while (*temp_strs)
		free(*temp_strs++);
	free(strs);
	return (NULL);
}
