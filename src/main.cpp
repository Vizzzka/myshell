#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "commands.h"
#include "parsing.h"
#include "config.h"


int main(int argc, char** argv) {

    //get commandline arguments
    if (get_commandline_options(argc, argv)) {
        return EXIT_SUCCESS;
    }
    // add current path to env
    add_path_to_env(":./:./external_commands");
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
        int is_end_of_file = !read_line_from_shell(line);
        // if script from command line ended
        if (is_end_of_file && conf_t.is_script()) {
            return EXIT_SUCCESS;
        }
        // if script ended then change rl streams to command line
        if (is_end_of_file) {
            rl_instream = stdin;
            rl_outstream = stdout;
            read_line_from_shell(line);
        }
        // parse line and get vector of arguments for each process
        std::vector<std::vector<std::string>> processes_args;
        parse_line(line, processes_args);
        if (processes_args.empty()) {
            continue;
        }

        // expand variables and replace wildcards for each proc
        for (auto& args: processes_args) {
            std::vector<std::string> new_args;
            expand_variables(args);
            new_args.push_back(args[0]);
            for (int i = 1; i < args.size(); ++i) {
                std::vector<std::string> arg = std::move(replace_wildcard_filename(args[i]));
                new_args.insert(new_args.end(), arg.begin(), arg.end());
            }
            args = new_args;
        }
        if (processes_args[0][0].empty()) {
            continue;
        }
        execute(processes_args);
    }
}
