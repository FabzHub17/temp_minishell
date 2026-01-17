/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilnassi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 11:47:02 by ilnassi           #+#    #+#             */
/*   Updated: 2026/01/17 12:23:25 by tvithara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* ===========================
**  INCLUDES
** =========================== */

# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

/* ATTENZIONE: aggiusta il path di libft in base al tuo progetto */
//# include "../libft/libft.h"
# include "../libft/includes/ft_printf.h"
# include "../libft/includes/libft.h"

/* ===========================
**  ENV WRAPPER (TUO)
** =========================== */

typedef struct s_envc
{
	char						**env;
	int							exit_code;
	int							should_exit;
	int							heredoc_id;
}								t_envc;

int								init_envc(t_envc *envc, char **envp);
void							free_envc(t_envc *envc);

/* ===========================
**  PARSER STRUCTS (FABIO)
** =========================== */

typedef enum e_token_type
{
	TK_WORD,
	TK_PIPE,
	TK_REDIR_IN,
	TK_REDIR_OUT,
	TK_REDIR_APP,
	TK_HEREDOC,
	TK_EOF
}								t_token_type;

// Token structure
typedef struct s_token
{
	t_token_type				type;
	char						*value;
	struct s_token				*next;
}								t_token;

// Command structure (for parsing results)
// 1 for >>, 0 for > (append mode)
typedef struct s_cmd
{
	char						**args;
	char						*input_file;
	char						*output_file;
	int							append_mode;
	char						*heredoc_delim;
	struct s_cmd				*next;
}								t_cmd;

// Main shell structure
typedef struct s_shell
{
	t_token						*tokens;
	t_cmd						*commands;
	//char			**env;
	t_envc						envc;
	int							exit_status;
}								t_shell;

// Expansion context structure
typedef struct s_expand_args
{
	int							exit_status;
	char						*result;
	t_shell						*shell;
}								t_expand_args;

/* ===========================
**  PARSER API (FABIO)
** =========================== */

// main
void							init_shell(t_shell *shell, char **envp);
void							cleanup_shell(t_shell *shell);

// lexer
t_token							*tokenize_input(char *input);
void							add_token(t_token **token_list,
									t_token_type type, char *value);
int								handle_redirection(char *input, int i,
									t_token **tokens);
int								handle_word(char *input, int i,
									t_token **tokens);
void							free_tokens(t_token *tokens);

// parser
t_cmd							*parse_tokens(t_token *tokens);
t_cmd							*create_command_node(void);
t_token							*handle_redir_parsing(t_token *token,
									t_cmd *cmd);
void							add_argument(t_cmd *cmd, char *arg);
void							free_commands(t_cmd *commands);
t_token							*handle_new_cmd(t_token *tk, t_cmd **cmds,
									t_cmd **current_cmd);
void							add_command(t_cmd **cmd_list, t_cmd *new_cmd);

// quote_handler
void							process_quotes_and_expansion(t_shell *shell);
char							*remove_quotes_and_expand(char *str,
									t_expand_args *expand_args);
int								handle_single_quotes(char *str, int i,
									t_expand_args *expand_args);
int								handle_double_quotes(char *str, int i,
									t_expand_args *expand_args);
int								expand_variable(char *str, int i,
									t_expand_args *expand_args);
char							*extract_var_name(char *str, int i);
char							*handle_heredoc_delimiter(char *str);

// utils
int								is_whitespace(char c);
int								is_metachar(char c);
int								is_redirection_token(t_token_type type);
char							*ft_strjoin_free(char *s1, char *s2);
void							print_error(char *msg);
int								skip_quoted_region(char *input, int i);
int								expand_env_variable(char *str, int i,
									t_expand_args *expand_args);

extern volatile sig_atomic_t	g_signal;

void							setup_signals_interactive(void);
void							setup_signals_child(void);
void							setup_signals_parent_exec(void);

#endif
