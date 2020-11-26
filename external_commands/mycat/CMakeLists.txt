cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME mycat)

project(mycat CXX)

set(CMAKE_CXX_STANDARD 17)

# Options
option(WARNINGS_AS_ERRORS   "Treat compiler warnings as errors."    OFF)
option(ENABLE_SANITIZERS    "Use sanitizers to detect errors."      OFF)
option(ENABLE_CONAN         "Use Conan as a package manager."       OFF)

# Include fixed CMake configuration
include(../../cmake/Config.cmake)

# Project source compilation
include_directories(inc)
add_library(file_io SHARED src/file_io.cpp inc/operations/file_io.hpp)


# Add external packages
find_package(Boost 1.65.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(mycat src/main.cpp)

target_link_libraries(mycat file_io)
target_link_libraries(mycat Boost::program_options)
message(${CMAKE_INSTALL_PREFIX})
install (TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
export(EXPORT ${PROJECT_NAME} FILE "${CMAKE_CURRENT_BINARY_DIR}/../${PROJECT_NAME}Targets.cmake")

# Sanitizers configuration
if (ENABLE_SANITIZERS)
    set(SANITIZE_UNDEFINED ON)
    # Only one of Memory, Address, or Thread sanitizers is applicable at the time
    set(SANITIZE_MEMORY ON)
#    set(SANITIZE_ADDRESS ON)
#    set(SANITIZE_THREAD ON)
    find_package(Sanitizers)

    # CMake targets to which apply sanitizers
    add_sanitizers(file_io add)
endif ()
