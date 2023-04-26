/*
	mpw-select.cc
	-------------
*/

// POSIX
#include <unistd.h>

// one_path
#include "one_path/find_MPW_dir.hh"


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( int argc, char** argv )
{
	int exit_status = 0;
	int fd          = STDOUT_FILENO;
	
	plus::string path = find_MPW_dir();
	
	const char* data;
	size_t      size;
	
	{
		data = path.data();
		size = path.size();
	}
	
	if ( ! wrote( fd, data, size ) )
	{
		return 13;
	}
	
	write( fd, "\n", 1 );
	
	return exit_status;
}
