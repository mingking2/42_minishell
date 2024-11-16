#include "minishell.h"
#include <stdlib.h>

char		**split_with_metachars(const char *s, char c);
static int	get_word_count(const char *s, char sep);

char	**split_with_metachars(const char *s, char c)
{
	char		**result;
	char		**r_temp;
	const char	*start;
	char		metachar;
	char		*metachar_loc;

	if (!s)
		return (NULL);
	result = (char **)malloc((get_word_count(s, c) + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	r_temp = result;
	start = s + (*s == c);
	metachar = 0;
	if (ft_strchr(META_CONTROL, *s))
		metachar = *s;
	while (*s++)
	{
		metachar_loc = ft_strchr(META_CONTROL, *s);
		if (*s == c || !*s || (!metachar && metachar_loc) || \
			(metachar && (!metachar_loc || metachar != *s)))
		{
			if (*(s - 1) != c)
			{
				*r_temp = ft_substr(start, 0, s - start);
				if (!*r_temp++)
					return (free_strs(result));
			}
			start = s + (*s == c);
			metachar = 0;
			if (metachar_loc)
				metachar = *s;
		}
	}
	*r_temp = NULL;
	return (result);
}

static int	get_word_count(const char *s, char sep)
{
	int		word_count;
	char	metachar;
	char	*metachar_loc;

	if (!s || ft_strchr(META_CONTROL, sep))
		return (-1);
	word_count = 0;
	metachar = 0;
	if (ft_strchr(META_CONTROL, *s))
		metachar = *s;
	while (*s++)
	{
		metachar_loc = ft_strchr(META_CONTROL, *s);
		if (((*s == sep || !*s) && *(s - 1) != sep) || \
			(metachar && (!metachar_loc || *s != metachar)))
		{
			metachar = 0;
			if (metachar_loc)
				metachar = *s;
			++word_count;
		}
		else if (!metachar && metachar_loc)
		{
			if (*(s - 1) != sep)
				++word_count;
			metachar = *s;
		}
	}
	return (word_count);
}
