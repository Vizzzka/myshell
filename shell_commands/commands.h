//
// Created by vizka on 20.11.20.
//

#ifndef ADDER_COMMANDS_H
#define ADDER_COMMANDS_H
#include <readline/readline.h>
#include <readline/history.h>
#include <map>
#include <vector>
#include <string>

extern int merrno_code;

bool get_options(const std::vector<std::string>& arguments, const std::string& usage_message, std::string& err_msg);

int merrno(const std::vector<std::string>& arguments);

int mpwd(const std::vector<std::string>& arguments);

int mcd(const std::vector<std::string>& arguments);

int mexit(const std::vector<std::string>& arguments);

int mecho(const std::vector<std::string>& arguments);

int mexport(const std::vector<std::string>& arguments);

int mexecute(const std::vector<std::string>& arguments);

#endif //ADDER_COMMANDS_H
