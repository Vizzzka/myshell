#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <map>
#include <vector>
#include <iostream>
#include <fnmatch.h>
#include <boost/algorithm/string.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "commands.h"
#include "parsing.h"
#include "error_functions.h"

// todo: fix merrno with scripts

namespace fs = boost::filesystem;
namespace po = boost::program_options;

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

bool is_redirection_symbol(std::string symbol) {
    return symbol == ">" || symbol == "2>" || symbol == "&>" || symbol == "<" || symbol == "2>&1";
}

void redirect(std::vector<std::string> redir_args) {
    if (redir_args.size() > 3 || redir_args.size() < 2) {
        err_exit("No valid redirect");
    }
    if (redir_args.size() == 3 && !is_redirection_symbol(redir_args[2])) {
        err_exit("No valid redirect");
    }
    std::string file_name = redir_args[1];

    if (redir_args[0] == "<") {
        int fd = open(file_name.c_str(), O_RDONLY);
        if (fd == -1) {
            err_exit("Unable open file for redirect");
        }
        dup2(fd, 0);  // Duplicate file handler -- redirect stdin
    }
    if (redir_args[0] == ">" && redir_args.size() == 2) {
        int fd = open(file_name.c_str(), O_WRONLY);
        if (fd == -1) {
            err_exit("Unable open file for redirect");
        }
        dup2(fd, 1);  // Duplicate file handler -- redirect stdout
    }

    if (redir_args[0] == "2>" && redir_args.size() == 2) {
        int fd = open(file_name.c_str(), O_WRONLY);
        if (fd == -1) {
            err_exit("Unable open file for redirect");
        }
        dup2(fd, 2);  // Duplicate file handler -- redirect stdout
    }

    if (redir_args[0] == "&>" || (redir_args.size() == 3 && redir_args[0] == ">" && redir_args[2] == "2>&1")) {
        int fd = open(file_name.c_str(), O_WRONLY);
        if (fd == -1) {
            err_exit("Unable open file for redirect");
        }
        dup2(fd, 2);  // Duplicate file handler -- redirect stderr
        dup2(fd, 1);
    }
}


int parse_line(std::string line, std::vector<std::vector<std::string>>& processes_args) {
    boost::trim(line);
    // get rid of comments
    std::string::size_type pos = line.find('#');
    if (pos != std::string::npos)
    {
        line = line.substr(0, pos);
    }
    // separate commands by |
    std::vector<std::string> processes_strings;
    boost::split(processes_strings, line, boost::is_any_of("|"));
    for (int i = 0; i < processes_strings.size(); ++i) {
        boost::trim(processes_strings[i]);
        // split command line by whitespace
        std::vector<std::string> v;
        boost::split(v, processes_strings[i], boost::is_any_of(" "));
        processes_args.emplace_back(v);
    }

    return 0;
}

void fork_exec(std::vector<std::string> arguments) {
    pid_t parent = getpid();
    std::string command = arguments[0];

    pid_t pid = fork();
    if (fs::path(command).extension() == ".msh") {
        arguments = std::vector<std::string>{"myshell", command};
        command = "myshell";
    }
    if (pid == -1)
    {
        err_exit("Failed to fork");
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        // We are the child
        exec_child_with_redirect(arguments);
        err_exit("Parent: Failed to execute\n");
    }
}

void dup_pipe_read(int* pfd) {
    if (close(pfd[1]) == -1) {
        err_exit("Failed to close pipe write entry\n");
    }
    if (pfd[0] != STDIN_FILENO) {       // defensive check
        if (dup2(pfd[0], STDIN_FILENO) == -1) {
            err_exit("Failed to dup2\n");
        }
        if (close(pfd[0]) == -1) {
            err_exit("Failed to close pipe read entry\n");
        }
    }
}

void dup_pipe_write(int* pfd) {
    if (close(pfd[0]) == -1) {
        err_exit("Failed to close pipe read entry");
    }
    if (pfd[1] != STDOUT_FILENO) {      // defensive check
        if (dup2(pfd[1], STDOUT_FILENO) == -1) {
            err_exit("Failed to dup2");
        }
        if (close(pfd[1]) == -1) {
            err_exit("Failed to close pipe write entry");
        }
    }
}

void exec_child_with_redirect(const std::vector<std::string>& args) {
    std::string command = args[0];
    std::vector<const char *> arg_for_c;
    for(int i = 0; i < args.size(); ++i) {
        if (is_redirection_symbol(args[i])) {
            redirect(std::vector<std::string>(args.begin() + i, args.end()));
            break;
        }
        arg_for_c.push_back(args[i].c_str());
    }
    arg_for_c.push_back(nullptr);

    execvp(command.c_str(), const_cast<char *const *>(arg_for_c.data()));
}

int execute(std::vector<std::vector<std::string>> processes_args) {
    bool in_background = false;
    int external_commands_count = 0;
    // create pipes
    int count_pipes = processes_args.size() - 1;
    int **pfd = new int *[count_pipes];
    for (int i = 0; i < count_pipes; ++i) {
        pfd[i] = new int[2];
    }

    for (int i = 0; i < count_pipes; ++i) {
        if (pipe(pfd[i]) == -1) {
            err_exit("Failed to create pipe");
            return -1;
        }
    }

    if (processes_args.back().back() == "&") {  // work in a background
        processes_args.back().pop_back();
        in_background = true;
    }

    // execute each process
    for (int i = 0; i < processes_args.size(); ++i) {
        std::string command = processes_args[i][0];
        if (internal_functions.find(command) != internal_functions.end()) {     // if command is internal
            if (i != count_pipes) {
                dup_pipe_write(pfd[i]);
            }
            if (i != 0) {
                dup_pipe_read(pfd[i - 1]);
            }
            internal_functions.find(command)->second(processes_args[i]);
            if (i != 0) {
                close(pfd[i - 1][0]);
                close(pfd[i - 1][1]);
            }
            continue;
        }
        external_commands_count++;
        if (fs::path(command).extension() == ".msh") {
            processes_args[i] = std::vector<std::string>{"myshell", command};
            command = "myshell";
        }
        switch(fork()) {        // if command is external then fork new proc
            case -1: {   // failed to fork
                err_exit("Parent: Failed to execute");
            }
            case 0: {     // child proc
                if (i != count_pipes) {
                    dup_pipe_write(pfd[i]);
                }
                if (i != 0) {
                    dup_pipe_read(pfd[i - 1]);
                }
                exec_child_with_redirect(processes_args[i]);
                err_exit("Parent: Failed to execute");
            }
            default:
                if (i != 0) {
                    close(pfd[i - 1][0]);
                    close(pfd[i - 1][1]);
                }
                break;
        }
    }
    // do not live zombies when parent ends
    if (in_background) {
        for (int i = 0; i < external_commands_count; ++i) {
            signal(SIGCHLD, SIG_IGN);
        }
        return 0;
    }

    // waiting for children
    for (int i = 0; i < external_commands_count; ++i) {
        if (wait(NULL) == -1) {
            err_exit("Failed to wait");
        }
    }
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

    if (strlen(line_buff) && fileno(rl_instream) == 0)
        add_history(line_buff);

    line = line_buff;
    free(line_buff);
    return true;
}

void expand_variables(std::vector<std::string>& arguments) {
    for (auto& arg : arguments) {
        if (arg[0] != '$') continue;
        std::string var_name = arg.substr(1);
        if (getenv(var_name.c_str())  != nullptr) {
            arg = getenv(var_name.c_str());
        }
        else {
            arg = "";
        }
    }
}

int add_path_to_env(const std::string& new_path) {
    auto path_ptr = getenv("PATH");
    std::string path_var;
    if (path_ptr != nullptr)
        path_var = path_ptr;
    path_var += new_path;
    setenv("PATH", path_var.c_str(), 1);
    return 0;
}
