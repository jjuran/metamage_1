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
	plus::string path = find_MPW_dir();
	
	char*  data = (char*) path.c_str();
	size_t size = path.size();
	
	data[ size++ ] = '\n';
	
	if ( ! wrote( STDOUT_FILENO, data, size ) )
	{
		return 13;
	}
	
	return 0;
}
