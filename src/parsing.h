#ifndef MYSHELL_PARSING_H
#define MYSHELL_PARSING_H
#include <vector>
#include <string>

typedef std::function<int (const std::vector<std::string> &)>  internal_func_type;

std::vector<std::string> replace_wildcard_filename(const std::string& file_name);

int get_commandline_options(int argc, char **argv);

int parse_line(std::string line, std::vector<std::vector<std::string>>& processes_args);

bool is_redirection_symbol(std::string symbol);

void redirect(std::vector<std::string> redir_args);

void exec_child_with_redirect(const std::vector<std::string>& args);

void fork_exec(std::vector<std::string> arguments);

void dup_pipe_write(int *pfd);

void dup_pipe_read(int *pfd);

int execute(std::vector<std::vector<std::string>> processes_args);

bool read_line_from_shell(std::string& line);

void expand_variables(std::vector<std::string>& arguments);

int add_path_to_env(const std::string& new_path);

#endif //MYSHELL_PARSING_H
