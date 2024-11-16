/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 03:46:35 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/26 03:46:36 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

char		*get_homedir(char *envp[]);
char		*get_euid_str(char *envp[]);
char		*get_hostname(char *envp[]);
char		*get_username(char *envp[]);
static int	exec_cmd_to_pipe(\
	char *cmd_path, char *const cmd_args[], char *const envp[], int pipe_fd[]);

char	*get_homedir(char *envp[])
{
	int		pipe_fd[2];
	char	*username;
	char	*user_info;
	char	**user_info_segments;
	char	*homedir;

	if (pipe(pipe_fd) == -1 && report_sys_err(1, "pipe", -1))
		return (NULL);
	username = get_username(envp);
	if (!username || exec_cmd_to_pipe(\
						"/usr/bin/getent", \
						(char *const []){"getent", "passwd", username, NULL}, \
						envp, pipe_fd) == -1)
		return (NULL);
	free(username);
	close(pipe_fd[1]);
	user_info = get_first_line(pipe_fd[0], 0);
	close(pipe_fd[0]);
	user_info_segments = ft_split(user_info, ':');
	free(user_info);
	if (!user_info_segments)
		return (NULL);
	homedir = ft_strdup(user_info_segments[5]);
	free_strs(user_info_segments);
	return (homedir);
}

char	*get_euid_str(char *envp[])
{
	int		pipe_fd[2];
	char	*euid_str;

	if (pipe(pipe_fd) == -1)
	{
		report_sys_err(1, "pipe", -1);
		return (NULL);
	}
	if (exec_cmd_to_pipe(\
			"/usr/bin/id", (char *const []){"id", "-u", NULL}, \
			envp, pipe_fd) == -1)
		return (NULL);
	close(pipe_fd[1]);
	euid_str = get_first_line(pipe_fd[0], 1);
	close(pipe_fd[0]);
	return (euid_str);
}

char	*get_hostname(char *envp[])
{
	int		pipe_fd[2];
	char	*hostname;

	if (pipe(pipe_fd) == -1)
	{
		report_sys_err(1, "pipe", -1);
		return (NULL);
	}
	if (exec_cmd_to_pipe(\
			"/usr/bin/hostname", (char *const []){"hostname", "-s", NULL}, \
			envp, pipe_fd) == -1)
		return (NULL);
	close(pipe_fd[1]);
	hostname = get_first_line(pipe_fd[0], 1);
	close(pipe_fd[0]);
	return (hostname);
}

char	*get_username(char *envp[])
{
	int		pipe_fd[2];
	char	*username;

	if (pipe(pipe_fd) == -1)
	{
		report_sys_err(1, "pipe", -1);
		return (NULL);
	}
	if (exec_cmd_to_pipe(\
			"/usr/bin/whoami", (char *const []){"whoami", NULL}, \
			envp, pipe_fd) == -1)
		return (NULL);
	close(pipe_fd[1]);
	username = get_first_line(pipe_fd[0], 1);
	close(pipe_fd[0]);
	return (username);
}

static int	exec_cmd_to_pipe(\
	char *cmd_path, char *const cmd_args[], char *const envp[], int pipe_fd[])
{
	pid_t	pid;
	int		wstatus;

	if (!cmd_path || !cmd_args || !envp || !pipe_fd)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (report_sys_err(1, "fork", -1));
	else if (pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		execve(cmd_path, cmd_args, envp);
		return (report_sys_err(1, cmd_args[0], -1));
	}
	waitpid(pid, &wstatus, 0);
	if (wstatus & 0xFF)
		return (-1);
	return (0);
}
