#include "../../errors_handling/error_functions.h"
#include <iostream>


void err_exit(const std::string& msg) {
    std::cerr << msg << "\n";
    exit(EXIT_FAILURE);
}