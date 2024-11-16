/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 20:52:50 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/16 20:53:05 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

int			selection_sort_strs(char **strs);
int			ft_strcmp(const char *s1, const char *s2);
char		*ft_substr(const char *s, unsigned int start, size_t len);
char		*ft_strchr(const char *s, int c);
char		*ft_strdup(const char *s);
size_t		ft_strlen(const char *s);
char		*ft_strjoin(const char *s1, const char *s2);
char		*strjoin_inplace(char **s1, const char *s2);
char		*strprepend_inplace(const char *s1, char **s2);
char		*strs_join(char **strs, const char *delimiter);
char		**dup_strs(char **strs);
char		**free_strs(char **strs);
long long	strict_atoll(char *nptr, int *is_err);
size_t		ft_strlcpy(char *dst, const char *src, size_t size);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		**filter_out_str(char **strs, const char *out_str);
char		*get_first_line(int fd, int del_newline_flag);
char		*ft_itoa(int n);
char		get_last_char(char *s);

int	selection_sort_strs(char **strs)
{
	int		i;
	int		j;
	int		min_idx;
	char	*temp;

	if (!strs)
		return (-1);
	if (!strs[0])
		return (0);
	i = -1;
	while (strs[++i + 1])
	{
		j = i;
		min_idx = i;
		while (strs[++j])
			if (ft_strcmp(strs[j], strs[min_idx]) < 0)
				min_idx = j;
		if (min_idx != i)
		{
			temp = strs[i];
			strs[i] = strs[min_idx];
			strs[min_idx] = temp;
		}
	}
	return (0);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	const unsigned char	*u_s1;
	const unsigned char	*u_s2;

	u_s1 = (const unsigned char *)s1;
	u_s2 = (const unsigned char *)s2;
	if (!u_s1 && !u_s2)
		return (0);
	else if (!u_s1 && u_s2)
		return (-*u_s2);
	else if (u_s1 && !u_s2)
		return (*u_s1);
	while (*u_s1 && *u_s2 && *u_s1 == *u_s2)
	{
		++u_s1;
		++u_s2;
	}
	return (*u_s1 - *u_s2);
}

char	*ft_substr(const char *s, unsigned int start, size_t len)
{
	char	*temp;
	size_t	s_len;
	char	*result;

	if (!s)
		return (NULL);
	while (*s && start-- > 0)
		++s;
	temp = (char *)s;
	s_len = 0;
	while (*temp++ && s_len < len)
		++s_len;
	result = (char *)malloc((s_len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	temp = result;
	while (*s && s_len-- > 0)
		*temp++ = *s++;
	*temp = '\0';
	return (result);
}

char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s && *s != c)
		++s;
	if (*s == c)
		return ((char *)s);
	return (NULL);
}

char	*ft_strdup(const char *s)
{
	char	*result;
	int		i;

	if (!s)
		return (NULL);
	result = (char *)malloc((ft_strlen(s) + 1) * sizeof(char));
	if (!result)
		return (NULL);
	i = -1;
	while (s[++i])
		result[i] = s[i];
	result[i] = '\0';
	return (result);
}

size_t	ft_strlen(const char *s)
{
	const char	*temp;

	if (!s)
		return (0);
	temp = s;
	while (*temp)
		++temp;
	return (temp - s);
}

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*result;
	char	*temp;

	if (!s1 && !s2)
		return (NULL);
	result = (char *)malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!result)
		return (NULL);
	temp = result;
	if (s1)
	{
		while (*s1)
			*temp++ = *s1++;
	}
	if (s2)
	{
		while (*s2)
			*temp++ = *s2++;
	}
	*temp = '\0';
	return (result);
}

char	*strjoin_inplace(char **s1, const char *s2)
{
	size_t	s1_len;
	size_t	s2_len;
	char	*result;

	if (!s1 || (!*s1 && !s2))
		return (NULL);
	s1_len = ft_strlen(*s1);
	s2_len = ft_strlen(s2);
	result = (char *)malloc((s1_len + s2_len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	ft_strlcpy(result, *s1, s1_len + 1);
	ft_strlcpy(result + s1_len, s2, s2_len + 1);
	free(*s1);
	*s1 = result;
	return (*s1);
}

char	*strprepend_inplace(const char *s1, char **s2)
{
	size_t	s1_len;
	size_t	s2_len;
	char	*result;

	if (!s2 || (!s1 && !*s2))
		return (NULL);
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(*s2);
	result = (char *)malloc((s1_len + s2_len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	ft_strlcpy(result, s1, s1_len + 1);
	ft_strlcpy(result + s1_len, *s2, s2_len + 1);
	free(*s2);
	*s2 = result;
	return (*s2);
}

char	*strs_join(char **strs, const char *delimiter)
{
	char	*result;
	int		i;

	if (!strs)
		return (NULL);
	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = -1;
	while (strs[++i])
	{
		if (!strjoin_inplace(&result, strs[i]) || \
			(delimiter && \
				strs[i + 1] && \
				!strjoin_inplace(&result, delimiter)))
		{
			free(result);
			return (NULL);
		}
	}
	return (result);
}

char	**dup_strs(char **strs)
{
	int		size;
	char	**result;
	int		i;

	size = -1;
	while (strs[++size])
		;
	result = (char **)malloc((size + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	i = -1;
	while (++i < size)
	{
		result[i] = ft_strdup(strs[i]);
		if (!result[i])
		{
			while (--i >= 0)
				free(result[i]);
			free(result);
			return (NULL);
		}
	}
	result[i] = NULL;
	return (result);
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

long long	strict_atoll(char *nptr, int *is_err)
{
	int			sign;
	long long	result;
	int			digit_count;

	if (!nptr || !is_err)
		return (-1);
	sign = 1;
	if ((*nptr == '+' || *nptr == '-') && *nptr++ == '-')
		sign = -1;
	*is_err = !*nptr;
	if (*is_err)
		return (-1);
	result = 0;
	digit_count = 0;
	while ('0' <= *nptr && *nptr <= '9')
	{
		if ((++digit_count >= 20 || \
				(digit_count == 19 && (*nptr - '0') > 7 + (sign == -1))) && \
			++*is_err)
			return (-1);
		result = result * 10 + (*nptr++ - '0');
	}
	if (*nptr && ++*is_err)
		return (-1);
	return (sign * result);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t		src_len;

	if (!dst || !src || !size)
		return (ft_strlen(src));
	src_len = 0;
	while (*src && size-- > 1)
	{
		*dst++ = *src++;
		++src_len;
	}
	while (*src++)
		++src_len;
	*dst = '\0';
	return (src_len);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char	*u_s1;
	const unsigned char	*u_s2;

	if (!n)
		return (0);
	u_s1 = (const unsigned char *)s1;
	u_s2 = (const unsigned char *)s2;
	if (!u_s1 && !u_s2)
		return (0);
	else if (!u_s1 && u_s2)
		return (-*u_s2);
	else if (u_s1 && !u_s2)
		return (*u_s1);
	while (*u_s1 && *u_s2 && *u_s1 == *u_s2 && n-- > 1)
	{
		u_s1++;
		u_s2++;
	}
	return (*u_s1 - *u_s2);
}

char	**filter_out_str(char **strs, const char *out_str)
{
	int		filtered_str_num;
	int		i;
	char	**result;
	int		j;

	if (!strs)
		return (NULL);
	filtered_str_num = 0;
	i = -1;
	while (strs[++i])
		if (ft_strcmp(strs[i], out_str))
			++filtered_str_num;
	result = (char **)ft_calloc(filtered_str_num + 1, sizeof(char *));
	if (!result)
		return (NULL);
	i = -1;
	j = -1;
	while (strs[++i])
		if (ft_strcmp(strs[i], out_str) && \
			!strjoin_inplace(&result[++j], strs[i]))
			break ;
	if (strs[i])
		return (free_strs(result));
	return (result);
}

char	*get_first_line(int fd, int del_newline_flag)
{
	int		is_err;
	char	*line;
	int		len;
	char	*temp;

	is_err = 0;
	line = get_next_line(fd, &is_err, 1);
	len = ft_strlen(line);
	if (del_newline_flag && line && *line && line[len - 1] == '\n')
	{
		temp = line;
		line = ft_substr(line, 0, len - 1);
		free(temp);
	}
	return (line);
}

char	*ft_itoa(int n)
{
	long long	abs_n;
	const int	is_negative = (n < 0);
	int			temp;
	int			len;
	char		*result;

	abs_n = n;
	if (is_negative)
		abs_n = -abs_n;
	temp = abs_n;
	len = 1;
	while (temp / 10 && ++len)
		temp /= 10;
	result = (char *)malloc((is_negative + len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	result[0] = '-';
	result[len + is_negative] = '\0';
	while (--len >= 0)
	{
		result[len + is_negative] = abs_n % 10 + '0';
		abs_n /= 10;
	}
	return (result);
}

char	get_last_char(char *s)
{
	if (!s || !*s)
		return (-1);
	return (s[ft_strlen(s) - 1]);
}
