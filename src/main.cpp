#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "commands.h"
#include "parsing.h"
#include "config.h"
#include <stdio.h>


int main(int argc, char** argv) {

    //get commandline arguments
    if (get_commandline_options(argc, argv)) {
        return EXIT_SUCCESS;
    }

    // add current path to env
    auto path_ptr = getenv("PATH");
    std::string path_var;
    if (path_ptr != nullptr)
        path_var = path_ptr;
    path_var += ":./:./external_commands";
    setenv("PATH", path_var.c_str(), 1);
    // config rl streams
    rl_instream = stdin;
    rl_outstream = stdout;
    // pass to config argc argv
    config conf_t(argc, argv);
    if (conf_t.is_script()) {
        std::string script_path, err_msg;
        if (conf_t.get_script_path(script_path, err_msg)) {
            std::cerr << err_msg;
            return EXIT_FAILURE;
        }

        rl_instream = fopen(script_path.data(), "r");
        rl_outstream = rl_instream;
        if (rl_instream == nullptr)
            std::cerr << "Failed to open: " << script_path;
    }

    // main loop
    while (true) {
        std::string line;
        int is_end_of_file = read_line_from_shell(line);
        // if script from command line ended
        if (!is_end_of_file && conf_t.is_script()) {
            return EXIT_SUCCESS;
        }
        // if script ended then change rl streams to command line
        if (!is_end_of_file) {
            rl_instream = stdin;
            rl_outstream = stdout;
            read_line_from_shell(line);
        }
        std::vector<std::string> arguments, new_arguments;
        parse_line(line, arguments);
        if (arguments[0].empty()) {
            continue;
        }
        expand_variables(arguments);
        new_arguments.push_back(arguments[0]);
        for (int i = 1; i < arguments.size(); ++i) {
            std::vector<std::string> arg = std::move(replace_wildcard_filename(arguments[i]));
            new_arguments.insert(new_arguments.end(), arg.begin(), arg.end());
        }
        execute(arguments[0], new_arguments);
    }

    return EXIT_SUCCESS;
}
