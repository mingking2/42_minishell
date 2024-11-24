#include "minishell.h"


char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t		i;

	if (little[0] == '\0')
		return ((char *)big);
	while (*big != '\0' && len > 0)
	{
		i = 0;
		while (*(big + i) == *(little + i) && i < len)
		{
			i++;
			if (*(little + i) == '\0')
				return ((char *)big);
		}
		big++;
		len--;
	}
	return (0);
}