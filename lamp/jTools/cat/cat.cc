/*
	cat.cc
	------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Relix
#include "relix/pump.h"

// more-posix
#include "more/perror.hh"


int main( int argc, char** argv )
{
	const char* argv0 = argv[0];
	
	const char *const * args = argv + 1;
	
	// Check for sufficient number of args
	if ( *args == NULL )
	{
		static const char *const default_args[] = { "-", NULL };
		
		args = default_args;
	}
	
	// Print each file in turn.  Return whether any errors occurred.
	int exit_status = 0;
	
	while ( *args != NULL )
	{
		const char* pathname = *args++;
		
		int fd;
		
		if ( pathname[ 0 ] == '-'  &&  pathname[ 1 ] == '\0' )
		{
			pathname = NULL;
			
			fd = STDIN_FILENO;
		}
		else
		{
			fd = open( pathname, O_RDONLY );
			
			if ( fd < 0 )
			{
				exit_status = 1;
				
				more::perror( argv0, pathname );
				continue;
			}
		}
		
		ssize_t n = pump( fd, NULL, STDOUT_FILENO, NULL, 0, 0 );
		
		if ( n < 0 )
		{
			exit_status = 1;
			
			more::perror( argv0, pathname );
		}
		
		if ( pathname != NULL )
		{
			close( fd );
		}
	}
	
	return exit_status;
}
