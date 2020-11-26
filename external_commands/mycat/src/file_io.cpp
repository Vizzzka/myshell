#include "operations/file_io.hpp"


#ifdef WIN32 // create_io_file for windows
HANDLE create_io_file(const char* file_name,
           DWORD desired_access,
           DWORD creation_despositions,
           DWORD flags_attributes)
{
        return ::CreateFileA(file_name,
                             desired_access,
                             FILE_SHARE_READ,
                             0,
                                creation_despositions,
                                flags_attributes,
                                0);
}
#elif __unix__ // create_io_file for Linux
HANDLE create_io_file(const char* file_name,
                      DWORD desired_access,
                      DWORD creation_despositions,
                      DWORD flags_attributes) {
    return ::open(file_name,
                  desired_access | creation_despositions,
                  flags_attributes);
}
#else
#error: unknown OS
#endif


#ifdef WIN32 // read_io_file for Windows
u_long read_io_file(HANDLE file_handle,
					LPVOID read_buffer,
					DWORD bytes_to_read) {
	u_long result_bytes = 0;
	BOOL result = ::ReadFile(file_handle,
							  read_buffer,
							  bytes_to_read,
							  &result_bytes,
							  0);

	if(!result)
		return -1;

	return result_bytes;
}

#elif __unix__ // read_io_file for Linux
ssize_t read_io_file(HANDLE file_handle,
                    LPVOID read_buffer,
                    DWORD bytes_to_read) {
    size_t read_bytes = 0;
    while(read_bytes < bytes_to_read) {
        ssize_t read_now = read(file_handle,
                                read_buffer + read_bytes,
                                bytes_to_read - read_bytes );
        if( read_now == -1) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else {
            read_bytes += read_now;
        }

        if (read_now == 0)
            break;
    }
    return read_bytes;
}
#else
#error: unknown OS
#endif

#ifdef WIN32 // write_io_file for Windows
u_long write_io_file(HANDLE file_handle,
					LPVOID read_buffer,
					DWORD bytes_to_write) {
	u_long result_bytes = 0;
	BOOL result = ::WriteFile(file_handle,
							  read_buffer,
							  bytes_to_write,
							  &result_bytes,
							  0);
	if(!result)
	{
		return -1;
	}

	return result_bytes;
}

#elif __unix__ // write_io_file for Linux
ssize_t write_io_file(HANDLE file_handle,
                     LPVOID read_buffer,
                     DWORD bytes_to_write) {
    size_t written_bytes = 0;
    while( written_bytes < bytes_to_write ) {
        ssize_t written_now = ::write(file_handle,
                                      read_buffer + written_bytes,
                                      bytes_to_write - written_bytes);
        if (written_now == -1) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else {
            written_bytes += written_now;
        }
    }
    return 0;
}
#endif

#ifdef WIN32

ssize_t write_io_file_hcode(HANDLE file_handle,
                            LPVOID read_buffer,
                            DWORD  bytes_to_write) {
	u_long result_bytes = 0;
	BOOL result = ::WriteFile(file_handle,
							  read_buffer,
							  bytes_to_write,
							  &result_bytes,
							  0);
	if(!result)
	{
		return -1;
	}

	return result_bytes;
}

#elif __unix__
ssize_t write_io_file_hcode(HANDLE file_handle,
                            LPVOID read_buffer,
                            DWORD  bytes_to_write) {
    size_t written_bytes = 0;
    while( written_bytes < bytes_to_write ) {

        size_t valid_bytes = 0;
        while((valid_bytes < bytes_to_write - written_bytes)
        && (isspace(read_buffer[written_bytes + valid_bytes])
        || isprint(read_buffer[written_bytes + valid_bytes]) != 0))
            valid_bytes++;
        if (write_io_file(file_handle,
                          read_buffer + written_bytes,
                          valid_bytes) < 0) {
            return -1;
        }
        written_bytes += valid_bytes;
        if (written_bytes == bytes_to_write)
            return 0;
        char hidden_ch = read_buffer[written_bytes];
        char h_buf[5];
        sprintf(h_buf, "\\x%02X", 0xFF & hidden_ch);
        write_io_file(file_handle, h_buf, 4);
        written_bytes += 1;
    }
    return 0;
}
#else
#error: Unknown OS
#endif

#ifdef WIN32 //close_io_file for Windows
bool close_io_file(HANDLE file_handle)
{
	return ::CloseHandle(file_handle);
}
#elif __unix__ //close_io_file for Linux
bool close_io_file(HANDLE file_handle) {

    if(close(file_handle) == -1)
        return false;

    return true;
}
#else
#error: unknown OS
#endif
