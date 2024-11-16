/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 23:55:28 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/16 23:55:36 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int	report_sys_err(int is_sh_err, const char *err_source, int err_code);
int	report_custom_err(\
		int is_sh_err, const char *err_source, const char *custom_err, \
		int err_code);
int	report_export_key_err(const char *env_pair, int err_code);

int	report_sys_err(int is_sh_err, const char *err_source, int err_code)
{
	char	*err_prefix;

	if (is_sh_err)
	{
		if (err_source)
		{
			err_prefix = ft_strjoin("bash: ", err_source);
			perror(err_prefix);
			free(err_prefix);
		}
		else
			perror("bash");
	}
	else
		perror(err_source);
	return (err_code);
}

int	report_custom_err(\
	int is_sh_err, const char *err_source, const char *custom_err, int err_code)
{
	char		*err_msg;
	int			i;
	char		*temp;
	const char	*err_strs[3] = {err_source, ": ", custom_err};

	if (is_sh_err)
		err_msg = ft_strdup("bash: ");
	else
		err_msg = NULL;
	if (err_source)
		i = -1;
	else
		i = 1;
	while (++i < 3)
	{
		temp = err_msg;
		err_msg = ft_strjoin(err_msg, err_strs[i]);
		free(temp);
	}
	write(STDERR_FILENO, err_msg, ft_strlen(err_msg));
	free(err_msg);
	return (err_code);
}

int	report_export_key_err(const char *env_pair, int err_code)
{
	char	*custom_err;

	custom_err = strs_join((char *[]){"`", (char *)env_pair, \
							"\': not a valid identifier\n", NULL}, \
							NULL);
	report_custom_err(1, "export", custom_err, -1);
	free(custom_err);
	return (err_code);
}
