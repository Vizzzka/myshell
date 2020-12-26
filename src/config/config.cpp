#include "config.h"
#include "boost/filesystem.hpp"
#include "../../src/parsing.h"
#include <iostream>

namespace fs = boost::filesystem;;

config::config(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        arguments.emplace_back(argv[i]);
    }
}

bool config::is_script() {
    return !arguments.empty() && !is_server;
}

int config::get_script_path(std::string& script_name, std::string& err_msg) {
    if (!is_script()) {
        return -1;
    }
    std::vector<std::string> script_paths;
    script_paths = replace_wildcard_filename(arguments[0]);

    fs::path script_path = fs::path(script_paths[0]);
    if (!fs::exists(script_path)) {
        err_msg = "No such file\n";
        return -1;
    }
    if (fs::extension(script_path) != script_ext) {
        err_msg = "Extension of script must me .msh";
        return -1;
    }
    script_name = script_path.string();
    return 0;
}

