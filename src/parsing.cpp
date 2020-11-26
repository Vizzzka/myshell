#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <map>
#include <iostream>
#include <fnmatch.h>
#include <boost/algorithm/string.hpp>
#ifdef __unix__
#include <sys/wait.h>
#elif WIN32
#include "windows.h"
#include "shlwapi.h"
#include <dirent.h>
#else: error unknown OS
#endif

#include "commands.h"
#include "parsing.h"


// todo: fix merrno with scripts

namespace fs = boost::filesystem;
namespace po = boost::program_options;
typedef std::function<int (const std::vector<std::string> &)>  internal_func_type;

const std::map<std::string, internal_func_type> internal_functions {
        { "merrno",  merrno},
        {"mpwd", mpwd},
        {"mcd", mcd},
        {"mexit", mexit},
        {"mecho", mecho},
        {"mexport", mexport},
        {".", mexecute}
};

std::vector<std::string> replace_wildcard_filename(const std::string& file_name) {
    std::vector<std::string> file_names;
    fs::directory_iterator end_itr;
    fs::path file_name_path = file_name;
    fs::path parent_dir = file_name_path.parent_path();
    if (!fs::is_directory(parent_dir)) {
        file_names.push_back(file_name);
        return file_names;
    }
    for (boost::filesystem::directory_iterator iter(parent_dir); iter != end_itr; ++iter) {
        if (!boost::filesystem::is_regular_file(iter->status())) continue; // Skip if not a file
        if (fnmatch(file_name_path.filename().c_str(), iter->path().filename().string().c_str(), 0) != 0) continue;
        file_names.push_back(iter->path().filename().string());
    }
    if (file_names.empty()) {
        file_names.push_back(file_name);
    }
    return file_names;
}

int get_commandline_options(int argc, char **argv) {
    po::options_description all("Supported options");
    all.add_options()
            ("help,h", "Print this help message.");
    try{
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(all).run(), vm);
        po::notify(vm);
        std::string help_message = "merrno [-h|--help] -- print exit code\n"
                                   "mpwd [-h|--help] -- print current path\n"
                                   "mcd <path> [-h|--help] -- go to the path\n"
                                   "mexit [exit_code] [-h|--help] -- exit myshell with exit_coe\n"
                                   "mecho [-h|--help] [text|$<var_name>] [text|$<var_name>]..."
                                   " -- print text or content of $var_name \n"
                                   "mexport var_name=VAL -- add global variable\n"
                                   " . <scriptname.msh> -- run the scriptname.msh in current interpreter\n";

        if (vm.count("help")) {
            std::cout << help_message << all << std::endl;
            return -1;
        }
    }
    catch (boost::wrapexcept<po::unknown_option> &_e) {}
    return 0;
}

int parse_line(const std::string& line, std::vector<std::string>& arguments) {
    std::string parsed_string;
    parsed_string = boost::algorithm::trim_copy(line);
    std::string::size_type pos = parsed_string.find('#');
    if (pos != std::string::npos)
    {
        parsed_string = parsed_string.substr(0, pos);
    }

    boost::split(arguments, parsed_string, boost::is_any_of(" "));

    return 0;
}

#ifdef __unix__
void fork_exec(std::string& command, std::vector<std::string>& arguments) {

    pid_t parent = getpid();
    pid_t pid = fork();
    if (fs::path(command).extension() == ".msh") {
        arguments = std::vector<std::string>{"myshell", command};
        command = "myshell";
    }
    if (pid == -1)
    {
        std::cerr << "Failed to fork()" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        // We are the child
        std::vector<const char*> arg_for_c;

        arg_for_c.reserve(arguments.size());
        for(const auto& s: arguments) {
            arg_for_c.push_back(s.c_str());
        }
        arg_for_c.push_back(nullptr);
        execvp(command.c_str(), const_cast<char* const *>(arg_for_c.data()));
        std::cerr << "Parent: Failed to execute " << command << " \n\tCode: " << errno << "\n";
        exit(EXIT_FAILURE);   // exec never returns
    }
}
#elif WIN32
void fork_exec(std::string& command, std::vector<std::string>& arguments) {

}
#else error: undefinded OS
#endif

int execute(std::string& command, std::vector<std::string>& arguments) {
    if (internal_functions.find(command) != internal_functions.end()) {
        return internal_functions.find(command)->second(arguments);
    }
    fork_exec(command, arguments);
    return 0;
}

bool read_line_from_shell(std::string& line) {
    char* line_buff;
    std::string invite_str;
    std::string line_str;
    if (fileno(rl_instream) == 0)
        invite_str = fs::current_path().string() + " $ ";

    line_buff = readline(invite_str.c_str());
    if (line_buff == nullptr) {
        line = "";
        return false;
    }

    if (!strlen(line_buff) && fileno(rl_instream) == 0)
        add_history(line_buff);

    line = line_buff;
    free(line_buff);
    return true;
}

void expand_variables(std::vector<std::string>& arguments) {
    for (auto& arg : arguments) {
        if (arg[0] != '$') continue;
        std::string var_name = arg.substr(1);
#ifdef __unix__
        if (getenv(var_name.c_str())  != nullptr) {
            arg = getenv(var_name.c_str());
        }
        else {
            arg = "";
        }
    }
#elif WIN32
    //GetEnvironmentVariable();
#else
#error: unknown OS
#endif
}

int add_path_to_env(const std::string& new_path) {
    auto path_ptr = getenv("PATH");
    std::string path_var;
    if (path_ptr != nullptr)
        path_var = path_ptr;
    path_var += new_path;
    setenv("PATH", path_var.c_str(), 1);
}