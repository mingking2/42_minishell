/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seonhwan <seonhwan@student.42gyeongsan.kr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 19:40:29 by seonhwan          #+#    #+#             */
/*   Updated: 2024/10/16 19:44:50 by seonhwan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stddef.h>
# include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

# ifndef GNL_BUFFER_SIZE
#  define GNL_BUFFER_SIZE 4096
# endif

# define GNL_HASH_TABLE_SIZE 1024
# define GNL_CACHE_SIZE 4096

# define METACHARS " \t\n|&;()<>"
# define META_WHITE_SPACE " \t\n"
# define META_CONTROL "|&;()<>"

typedef enum e_redir_type       t_redir_type;

typedef struct s_file_node		t_file_node;
typedef struct s_shell_info		t_shell_info;
typedef struct s_env_list		t_env_list;
typedef struct s_quote_node		t_quote_node;
typedef struct s_pipeline_cmd   t_pipeline_cmd;
typedef struct s_redir_list     t_redir_list;

enum e_redir_type
{
    IN = 0,
    OUT,
	APPEND,
    HEREDOC
};

struct s_file_node
{
	int					fd;
	char				*str;
	struct s_file_node	*next;
};

struct s_shell_info
{
	char			**initital_env;
	t_env_list		*env_list;
	t_env_list		*underscore_env;
	t_env_list		*last_env;
	t_env_list		*sorted_env_list;
	int				last_exit_status;
	char			*cwd;
	char			*homedir;
	char			*user_host_segment;
	const char		*admin_priv_segment;
	char			*prompt;
	char			*user_input;
	t_quote_node	*quote_list;
};

struct s_env_list
{
	char				*key;
	char				*value;
	struct s_env_list	*next;
};

struct s_quote_node
{
	char				*raw_str;
	char				quote;
	char				**tokens;
	int					link_to_next;
	struct s_quote_node	*next;
};

struct s_pipeline_cmd
{
    char                    **tokens;
    t_redir_list            *redir_list;
    struct s_pipeline_cmd   *next;
};

struct s_redir_list
{
    t_redir_type		type;
    char				*arg;
	char				*limiter;
    struct s_redir_list	*next;
};

// builtin.c
int			echo(t_shell_info *shell, char **args);
int			cd(t_shell_info *shell, char **args);
int			pwd(t_shell_info *shell, char **args);
int			export(t_shell_info *shell, char **args);
int			unset(t_shell_info *shell, char **args);
int			env(t_shell_info *shell, char **args);
void		bash_exit(t_shell_info *shell, char **args);

// char_checks.c
int			ft_isalpha(int c);
int			ft_isdigit(int c);
int			ft_isalnum(int c);
int			ft_isspace(int c);
int			isquote(int c);

// clean_path.c
char		*clean_path(char *abs_path);

// env_management.c
t_env_list	*new_env_list(char *envp[]);
t_env_list	*del_env_list(t_env_list *env);
int			update_env_list(t_shell_info *shell, char *env_pair);
int			is_valid_env_key(char *key);
char		*get_env_value(t_env_list *env, char *key);
int			replace_env_value(\
		t_shell_info *shell, char *key, char *value, int append_flag);
t_env_list	*search_env(t_env_list *env, char *key);
t_env_list	*extract_env(t_env_list **env_ptr_addr, char *key);
int			set_underscore_env(t_shell_info *shell, char *cmd, char **args);

// error_print.c
int			report_sys_err(int is_sh_err, const char *err_source, int err_code);
int			report_custom_err(\
		int is_sh_err, const char *err_source, const char *custom_err, \
		int err_code);
int			report_export_key_err(const char *env_pair, int err_code);

// ft_split.c
char		**ft_split(const char *s, char c);

// get_next_line.c
char		*get_next_line(int fd, int *is_err, int del_node_flag);

// memory_utils.c
void		*ft_memset(void *s, int c, size_t n);
void		*ft_calloc(size_t nmemb, size_t size);
void		*free_and_return_null(void *ptr);
char		**free_strs(char **strs);

// parse_quotes.c
char			**tokenize_input(t_shell_info *shell);
t_quote_node	*del_quote_list(t_quote_node **quote_list);

// shell_management.c
int			init_shell(t_shell_info *shell, char *envp[]);
int			clear_shell(t_shell_info *shell, int return_value);

// shell_utils.c
char		*get_homedir(char *envp[]);
char		*get_euid_str(char *envp[]);
char		*get_hostname(char *envp[]);
char		*get_username(char *envp[]);

// split_with_metachars.c
char		**split_with_metachars(const char *s, char c);

// str_utils.c
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
long long	strict_atoll(char *nptr, int *is_err);
size_t		ft_strlcpy(char *dst, const char *src, size_t size);
int			ft_strncmp(const char *s1, const char *s2, size_t n);
char		**filter_out_str(char **strs, const char *out_str);
char		*get_first_line(int fd, int del_newline_flag);
char		*ft_itoa(int n);
char		get_last_char(const char *s);
char		*search_first_non_whitespace(const char *s);
int			get_strs_num(char **strs);

//pipe_utils.c
char	*ft_strnstr(const char *big, const char *little, size_t len);

//pipex.c
void pipe_execute(t_shell_info *shell, char **envp);

#endif
