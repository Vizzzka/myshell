cmake_minimum_required(VERSION 3.15)
project(myshell
        VERSION 1.0.0
        DESCRIPTION "simple command shell"
        HOMEPAGE_URL "https://github.com/chernetskyi/cpp-template"
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Options
option(WARNINGS_AS_ERRORS   "Treat compiler warnings as errors."    ON)
option(ENABLE_CONAN         "Use Conan as a package manager."       OFF)
option(ENABLE_PVS_STUDIO    "Use PVS-Studio static code analyzer."  OFF) # Option for the local usage only. PVS-Studio isn't installed on GitHub machines.
option(ENABLE_SANITIZERS    "Use sanitizers to detect errors."      OFF) # Option for the test builds. Do not use it in the production builds.

add_subdirectory(external_commands/mycat ${CMAKE_CURRENT_BINARY_DIR}/external_commands)
# Project source compilation
include_directories(shell_commands)
include_directories(config)
add_library(shell_commands STATIC
        src/shell_commands/commands.cpp shell_commands/commands.h)
add_executable(myshell src/main.cpp config/config.h src/config/config.cpp src/parsing.cpp src/parsing.h)
target_link_libraries(myshell shell_commands)

# Add external programs
install(EXPORT mycat DESTINATION ${PROJECT_SOURCE_DIR}/bin)

# Add external packages
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.71.0 COMPONENTS program_options filesystem REQUIRED)
find_library(READLINE_LIB readline)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(myshell Boost::program_options Boost::filesystem)
target_link_libraries(myshell  readline)
# Define ALL_TARGETS variable to use in some directives
set(ALL_TARGETS arithmetic myshell)

# Include default CMake configuration
include(cmake/Config.cmake)
