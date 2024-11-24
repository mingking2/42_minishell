#include "minishell.h"

void	error(void)
{
	perror("\033[31mError");
	exit(EXIT_FAILURE);
}

char *find_path(char *cmd, char **envp) {
    char    **paths;
    char    *path;
    int     i;
    char    *part_path;

    i = 0;
    while(ft_strnstr(envp[i], "PATH", 4) == 0)
        i++;
    paths = ft_split(envp[i] + 5, ':');
    i = 0;
    while (paths[i])
    {
        part_path = ft_strjoin(paths[i], "/");
        path = ft_strjoin(part_path, cmd);
        free(part_path);
        if (access(path, F_OK) == 0)
            return path;
        free(path);
        i++;
    }
    i = -1;
    while(paths[++i])
        free(paths[i]);
    free(path);
    return (0);
}


void execute(char *argv, char **envp) {
    char    **cmd;
    int     i;
    char    *path;

    i = -1;
    cmd = ft_split(argv, ' ');
    path = find_path(cmd[0], envp);
    if (!path)
    {
        while (cmd[++i])
            free(cmd[i]);
        free(cmd);
        error();
    }
    if (execve(path, cmd, envp) == -1)
    {
        error();
    }
}

int execute_builtin(t_shell_info *shell, char *command)
{
    char **line_segments;
    char **exit_args;

    line_segments = ft_split(command, ' ');
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
    {
        free_strs(line_segments);
        return (-1); // 지원하지 않는 명령어
    }

    free_strs(line_segments);
    return (0); // 내장 명령어 처리 완료
}


void pipe_execute(t_shell_info *shell, char **envp)
{
    char **commands;
    int **pipes;
    int num_commands;
    pid_t pid;
    int i;
    int status = 0;

    // 1. 명령어 파싱
    commands = ft_split(shell->user_input, '|');
    if (!commands)
        error();

    num_commands = 0;
    while (commands[num_commands])
        num_commands++;

    // 2. 파이프 생성
    pipes = malloc(sizeof(int *) * (num_commands - 1));
    for (i = 0; i < num_commands - 1; i++)
    {
        pipes[i] = malloc(sizeof(int) * 2);
        if (pipe(pipes[i]) == -1)
            error();
    }

    // 3. 명령어 실행
    for (i = 0; i < num_commands; i++)
    {
        pid = fork();
        if (pid == -1)
            error();

        if (pid == 0) // 자식 프로세스
        {
            // 이전 명령어의 출력 연결
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
            }

            // 다음 명령어의 입력 연결
            if (i < num_commands - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
            }

            // 모든 파이프 닫기
            for (int j = 0; j < num_commands - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            
            if (execute_builtin(shell, commands[i]) == -1)
            {
                // 내장 명령어가 아니면 외부 명령어 실행
                execute(commands[i], envp);
            }

            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < num_commands; i++)
        wait(&status);

    // 4. 부모 프로세스: 파이프 닫기 및 대기
    for (i = 0; i < num_commands - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // 메모리 해제
    for (i = 0; i < num_commands - 1; i++)
        free(pipes[i]);
    free(pipes);
    free_strs(commands);


    return (status);
}