#ifndef ADDER_OPERATIONS_HPP
#define ADDER_OPERATIONS_HPP


typedef unsigned char u_char;
typedef unsigned long u_long;
typedef unsigned long long u_longlong;


#ifdef WIN32
#include <Windows.h>
typedef __int64 ssize_t;
#elif __linux__
#define INVALID_HANDLE_VALUE       -1		//determine parameter for error
typedef int HANDLE;							//define type of file descriptor
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef char * LPVOID;
#else
#error: unknown OS
#endif

#ifdef WIN32
// nothing to do
#elif __unix__
#include <cerrno>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "unistd.h"
#include <fcntl.h>
#include <stdio.h>

#define GENERIC_READ                O_RDONLY	//read only mode
#define GENERIC_WRITE               O_WRONLY	//write only mode
#define CREATE_ALWAYS               O_CREAT		//create new file
#define OPEN_EXISTING               0           //fake parameter's value
#define FILE_ATTRIBUTE_NORMAL     0644          // file attributes
#else
#error: unknown OS
#endif


HANDLE create_io_file(const char* file_name,
                      DWORD desired_access,
                      DWORD creation_despositions,
                      DWORD flags_attributes);

ssize_t read_io_file(HANDLE file_handle,
                    LPVOID read_buffer,
                    DWORD  bytes_to_read);

ssize_t write_io_file(HANDLE file_handle,
                      LPVOID read_buffer,
                      DWORD  bytes_to_write);

ssize_t write_io_file_hcode(HANDLE file_handle,
                            LPVOID read_buffer,
                            DWORD  bytes_to_write);

bool close_io_file(HANDLE file_handle);


#endif //ADDER_OPERATIONS_HPP
