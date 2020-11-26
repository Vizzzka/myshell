#ifndef MYSHELL_PARSING_H
#define MYSHELL_PARSING_H
#include <vector>
#include <string>

std::vector<std::string> replace_wildcard_filename(const std::string& file_name);

int get_commandline_options(int argc, char **argv);

int parse_line(const std::string& line, std::vector<std::string>& arguments);

int execute(std::string& command, std::vector<std::string>& arguments);

bool read_line_from_shell(std::string& line);

void expand_variables(std::vector<std::string>& arguments);

#endif //MYSHELL_PARSING_H
