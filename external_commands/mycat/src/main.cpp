#include <iostream>
#include <boost/program_options.hpp>
#include <vector>
#include "operations/file_io.hpp"


const size_t bytesPerRead = 1024;
const int STDOUT_FD = 1;
const int STDERR_FD = 2;

int openFiles(const std::vector<std::string>& file_names,
              std::vector<int>& file_descriptors)
{
    for (const auto& fileName: file_names) {
        int fd = create_io_file(fileName.c_str(),
                                GENERIC_READ,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL);
        if (fd < 0) {
            char* err_buff = &strerror(errno) [ '\0'];
            write_io_file(STDERR_FD, err_buff, strlen(err_buff));
            return -1;
        }
        file_descriptors.push_back(fd);
    }
    return 0;
}


int readFiles(const std::vector<int>& file_descriptors, bool flagA) {
    for (const auto& fd: file_descriptors) {
        char* buffer = new char [bytesPerRead];
        ssize_t read_bytes = 1;

        while(read_bytes > 0) {
            read_bytes = read_io_file(fd, buffer, bytesPerRead);
            if (read_bytes < 0) {
                char* err_buff = &strerror(errno) [ '\0'];
                write_io_file(STDERR_FD, err_buff, strlen(err_buff));
                delete[] buffer;
                return -1;
            }
            if (flagA)
                write_io_file_hcode(STDOUT_FD, buffer, read_bytes);
            else
                write_io_file(STDOUT_FD, buffer, read_bytes);
        }

        delete[] buffer;
        close_io_file(fd);
    }
    return 0;
}


int main(int argc, char **argv) {
    std::vector<std::string> file_names;
    std::vector<int> file_descriptors;
    bool flagA = false;
    namespace po = boost::program_options;

    po::options_description visible("Supported options");
    visible.add_options()
            ("help,h", "Print this help message.")
            ("A,A", "Show hidden symbols as hex codes");

    po::options_description hidden("Hidden options");
    hidden.add_options()
            ("input-files", po::value<std::vector<std::string>>(), "input-files");

    po::positional_options_description p;
    p.add("input-files", -1);

    po::options_description all("All options");
    all.add(visible).add(hidden);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(all).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << visible << std::endl;
        return EXIT_SUCCESS;
    }
    if (vm.count("A")) {
        flagA = true;
    }
    file_names = vm["input-files"].as< std::vector<std::string> >();
    int status = openFiles(file_names, file_descriptors);
    if (status != 0) {
        return EXIT_FAILURE;
    }

    status = readFiles(file_descriptors, flagA);
    if (status != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
