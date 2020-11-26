#include "commands.h"
#include <iostream>
#include <boost/filesystem.hpp>
#ifdef __unix__
#include <sys/wait.h>
#include "unistd.h"
#include <stdlib.h>
#include "boost/program_options.hpp"
#elif WIN32
#include <direct.h>
#else
#error: unknown OS
#endif

namespace fs = boost::filesystem;
namespace po = boost::program_options;
int merrno_code;
bool is_help;
//todo mcd error
bool get_options(const std::vector<std::string>& arguments, const std::string& usage_message, std::string& err_msg) {
    bool no_wrong_option = true;
    po::options_description all("Supported options");
    all.add_options()
            ("help,h", "Print this help message.");
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(arguments).options(all).run(), vm);
    } catch (boost::wrapexcept<po::unknown_option> &_e) {
        no_wrong_option = false;
        err_msg = (*&_e).get_option_name() + ": invalid option\n";
    }
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << usage_message << "\n";
        is_help = true;
    }
    else {
        is_help = false;
    }
    return no_wrong_option;
}

int merrno(const std::vector<std::string>& arguments) {
    std::string usage = "merrno: usage: merrno [-h|--help]\n";
    std::string err_msg;
    if (!get_options(arguments, usage, err_msg)) {
        std::cout << err_msg << usage;
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }
    if (is_help) {
        merrno_code = EXIT_SUCCESS;
        return merrno_code;
    }
    std::cout << merrno_code << "\n";
    merrno_code = EXIT_SUCCESS;
    return merrno_code;
}

int mpwd(const std::vector<std::string>& arguments) {
    std::string usage = "merrno: usage: mpwd [-h|--help]\n";
    std::string err_msg;
    if (!get_options(arguments, usage, err_msg)) {
        std::cout << err_msg << usage;
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }
    if (is_help) {
        merrno_code = EXIT_SUCCESS;
        return merrno_code;
    }
    merrno_code = EXIT_SUCCESS;
    std::cout << fs::current_path().string() << "\n";
    return merrno_code;
}

int mcd(const std::vector<std::string>& arguments) {
    std::string usage = "mcd: usage: mcd <path> [-h|--help] \n";
    std::string err_msg;
    if (!get_options(arguments, usage, err_msg)) {
        std::cout << err_msg << usage;
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }
    if (is_help || arguments.size() == 1) {
        merrno_code = EXIT_SUCCESS;
        return merrno_code;
    }

    merrno_code = chdir(arguments[1].c_str()) ? EXIT_FAILURE : EXIT_SUCCESS;
    return merrno_code;
}

int mexit(const std::vector<std::string>& arguments) {
    std::string usage = "mexit: usage: mexit [exit code] [-h|--help] \n";
    std::string err_msg;
    if (!get_options(arguments, usage, err_msg)) {
        std::cout << err_msg << usage;
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }
    if (is_help) {
        merrno_code = EXIT_SUCCESS;
        exit(merrno_code);
    }

    try {
        int status = atoi(arguments[0].c_str());
        merrno_code = status;
        exit(status);
    }
    catch (std::exception const & e) {
        merrno_code = EXIT_FAILURE;
        exit(merrno_code);
    }
}

int mecho(const std::vector<std::string>& arguments) {
    std::string usage = "mecho: usage: mecho [-h|--help] [text|$<var_name>] [text|$<var_name>] \n";
    std::string err_msg;
    get_options(arguments, usage, err_msg);
    if (is_help) {
        merrno_code = EXIT_SUCCESS;
        return merrno_code;
    }
    for (int i = 1; i < arguments.size(); ++i) {
        if (arguments[i].empty())
            continue;
        std::cout << arguments[i] << " ";
    }
    std::cout << "\n";
    merrno_code = EXIT_SUCCESS;
    return merrno_code;
}

int mexport(const std::vector<std::string>& arguments) {
    std::string usage = "mexport: usage: mexport var_name=VAL \n";
    std::string err_msg;
    if (!get_options(arguments, usage, err_msg)) {
        std::cout << err_msg << usage;
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }
    if (is_help || arguments.size() == 1) {
        merrno_code = EXIT_SUCCESS;
        return merrno_code;
    }
    std::string var_name, var_value, var;
    var = arguments[1];
    std::size_t pos = var.find_first_of('=');
    if (pos == std::string::npos) {
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }
    var_name = var.substr(0, pos);
    var_value = var.substr(pos + 1);
    merrno_code = setenv(var_name.c_str(), var_value.c_str(), 1);
    return merrno_code;
}

int mexecute(const std::vector<std::string>& arguments) {
    if (arguments.size() == 1) {
        std::cerr << "No script provided\n";
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }
    std::string script_name = arguments[1];
    if (!fs::exists(fs::path(script_name)) || fs::path(script_name).extension() != ".msh") {
        merrno_code = EXIT_FAILURE;
        return merrno_code;
    }

    rl_instream = fopen(script_name.data(), "r");
    rl_outstream = rl_instream;
    //merrno_code = EXIT_SUCCESS; -- ???
    return merrno_code;
}


