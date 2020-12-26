#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "commands.h"
#include "parsing.h"
#include "config.h"


int main(int argc, char** argv) {
    p_config = new config(argc, argv);
    //get commandline arguments
    if (get_commandline_options(argc, argv)) {
        return EXIT_SUCCESS;
    }
    // add current path to env
    add_path_to_env(":./:./external_commands");
    // config rl streams
    rl_instream = stdin;
    rl_outstream = stdout;

    if (p_config->is_script()) {
        std::string script_path, err_msg;
        if (p_config->get_script_path(script_path, err_msg)) {
            std::cerr << err_msg;
            return EXIT_FAILURE;
        }

        rl_instream = fopen(script_path.data(), "r");
        rl_outstream = rl_instream;
        if (rl_instream == nullptr)
            std::cerr << "Failed to open: " << script_path;
    }

    if (p_config->is_server) {
        multiple_connection_server(p_config->port);
        return EXIT_SUCCESS;
    }

    // main loop
    return main_loop(0);
}
