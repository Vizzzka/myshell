#ifndef MYSHELL_CONFIG_H
#define MYSHELL_CONFIG_H

#include <string>
#include <vector>

struct config{
    const std::string script_ext = ".msh";

    int get_script_path(std::string& script_name, std::string& err_msg);
    bool is_script();
    std::vector<std::string> arguments;
    config(int argc, char *argv[]);
    ~config() = default;
};
#endif //MYSHELL_CONFIG_H
