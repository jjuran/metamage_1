/*
	rmdir.cc
	--------
*/

// POSIX
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define PREFIX  "rmdir: "

#define STR_LEN( s ) "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( int argc, char *const *argv )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		return ERROR( 50, "missing arguments" );
	}
	
	int exit_status = 0;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		const char* pathname = argv[ index ];
		
		int removed = rmdir( pathname );
		
		if ( removed < 0 )
		{
			exit_status = 1;
			
			more::perror( "rmdir", argv[ index ] );
		}
	}
	
	return exit_status;
}
