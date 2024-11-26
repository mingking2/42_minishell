/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 18:25:46 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/26 13:40:21 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <unistd.h>

static char	*update_prompt(t_shell_info *shell);
static char	*convert_home_to_tilde(t_shell_info *shell, char *dir);
static int	test_builtin(t_shell_info *shell);
// void		test(t_shell_info shell);

int	main(int argc, char *argv[], char *envp[])
{
	t_shell_info	shell;
	int				is_prompt_update_err;
	char			**tokens;
	int				i;

	(void)argc;
	(void)argv;
	printf("시작");
	if (init_shell(&shell, envp) == -1)
		return (EXIT_FAILURE);
	shell.user_input = readline(update_prompt(&shell));
	while (shell.user_input && shell.prompt)
	{
		if (*shell.user_input)
			add_history(shell.user_input);
		
		test_builtin(&shell);
		free(shell.user_input);
		shell.user_input = readline(update_prompt(&shell));
	}
	is_prompt_update_err = !shell.prompt;
	clear_shell(&shell, 0);
	if (is_prompt_update_err)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static char	*update_prompt(t_shell_info *shell)
{
	char	*cwd;
	char	*temp;

	free(shell->prompt);
	shell->prompt = NULL;
	cwd = convert_home_to_tilde(shell, get_env_value(shell->env_list, "PWD"));
	if (!cwd)
	{
		temp = getcwd(NULL, PATH_MAX);
		if (!temp)
		{
			report_sys_err(1, "getcwd", -1);
			return (NULL);
		}
		cwd = convert_home_to_tilde(shell, temp);
		free(temp);
	}
	temp = ft_strjoin(shell->user_host_segment, cwd);
	free(cwd);
	if (!temp)
		return (NULL);
	shell->prompt = ft_strjoin(temp, shell->admin_priv_segment);
	free(temp);
	return (shell->prompt);
}

static char	*convert_home_to_tilde(t_shell_info *shell, char *dir)
{
	char	*home;
	int		i;
	int		home_len;

	if (!shell || !dir)
		return (NULL);
	home = get_env_value(shell->env_list, "HOME");
	if (!home || !ft_strcmp(home, "/") || !ft_strcmp(home, "//"))
		return (ft_strdup(dir));
	i = -1;
	while (home[++i] == '/')
		;
	home_len = ft_strlen(home);
	if (i == 0 || i > 2 || home[home_len - 1] == '/')
		return (ft_strdup(dir));
	if (!ft_strncmp(dir, home, home_len) && \
		(dir[home_len] == '\0' || dir[home_len] == '/'))
		return (ft_strjoin("~", &dir[home_len]));
	return (ft_strdup(dir));
}

int	test_builtin(t_shell_info *shell)
{
	char	**line_segments;
	char	**exit_args;

	if (!shell)
		return (-1);
	printf("\nshell->user_input : %s\n", shell->user_input);
	line_segments = ft_split(shell->user_input, ' ');
	if (!line_segments)
		return (-1);
	if (!ft_strcmp(line_segments[0], "echo"))
		echo(shell, &line_segments[1]);
	else if (!ft_strcmp(line_segments[0], "cd"))
		cd(shell, &line_segments[1]);
	else if (!ft_strcmp(line_segments[0], "pwd"))
		pwd(shell, &line_segments[1]);
	else if (!ft_strcmp(line_segments[0], "export"))
		export(shell, &line_segments[1]);
	else if (!ft_strcmp(line_segments[0], "unset"))
		unset(shell, &line_segments[1]);
	else if (!ft_strcmp(line_segments[0], "env"))
		env(shell, &line_segments[1]);
	else if (!ft_strcmp(line_segments[0], "exit"))
	{
		exit_args = dup_strs(&line_segments[1]);
		free_strs(line_segments);
		bash_exit(shell, exit_args);
	}
	else
		printf("do nothing\n");
	free_strs(line_segments);
	return (0);
}




// void	test(t_shell_info shell)
// {
// 	printf("1 ------------------------------------------------------------\n");
// 	export(&shell, NULL);
// 	printf("2 ------------------------------------------------------------\n");
// 	export(&shell, (char *[]){"A=10", "B=", "C", "=", "=3", "D=123", NULL});
// 	printf("3 ------------------------------------------------------------\n");
// 	export(&shell, NULL);
// 	export(&shell, NULL);
// 	printf("4 ------------------------------------------------------------\n");
// 	env(&shell, NULL);
// 	pwd(&shell, (char *[]){"asdsd", "abc", NULL});
// 	export(&shell, (char *[]){"A=10", "B=", "C", "=", "=3", "D=123", NULL});
// 	export(&shell, NULL);
// 	unset(&shell, (char *[]){"D", "B", NULL});
// 	export(&shell, NULL);
// 	printf("$_ : %s \n", shell.underscore_env->value);
// 	pwd(&shell, (char *[]){"asdsd", "abc", NULL});
// 	cd(&shell, (char *[]){"abc", NULL});
// 	pwd(&shell, (char *[]){"asdsd", "abc", NULL});
// 	echo(&shell, (char *[]){"-n", "-nnnnn", "-n", "-nn", "a", "", "asdsd", "abc", NULL});
// 	echo(&shell, (char *[]){"-n", NULL});
// 	printf("shell.underscore : %s \n", shell.underscore_env->value);
// 	pwd(&shell, NULL);
// 	cd(&shell, (char *[]){"~", NULL});

// 	export(&shell, (char *[]){"a+=12", NULL});
// 	export(&shell, (char *[]){NULL});
// 	printf("\n\n-------------------------------------------------\n\n\n");
// 	export(&shell, (char *[]){"a+=34", NULL});
// 	export(&shell, (char *[]){NULL});
// 	printf("\n\n-------------------------------------------------\n\n\n");
// 	export(&shell, (char *[]){"a++=56", NULL});
// 	export(&shell, (char *[]){NULL});
// 	printf("\n\n-------------------------------------------------\n\n\n");
// 	export(&shell, (char *[]){"b=abc", NULL});
// 	export(&shell, (char *[]){NULL});
// 	printf("\n\n-------------------------------------------------\n\n\n");
// 	unset(&shell, (char *[]){"a", NULL});
// 	export(&shell, (char *[]){NULL});

// 	printf("homedir : %s \n", shell.homedir);
// 	pwd(&shell, NULL);
// }
