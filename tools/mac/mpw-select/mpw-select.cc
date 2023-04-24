/*
	mpw-select.cc
	-------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// vxo
#include "vxo/error.hh"

// plus
#include "plus/string.hh"

// one_path
#include "one_path/find_MPW_dir.hh"


#pragma exceptions off


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( int argc, char** argv )
{
	vxo::Box xstring = find_MPW_dir();
	
	int exit_status = 0;
	int fd          = STDOUT_FILENO;
	
	const char* data;
	size_t      size;
	
	if ( const vxo::Error* error = xstring.is< vxo::Error >() )
	{
		exit_status = 43;
		
		fd = STDERR_FILENO;
		
		data = error->get();
		size = strlen( data );
	}
	else
	{
		const plus::string& path = *(const plus::string*) &xstring;
		
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
