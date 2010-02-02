/*
	clear.cc
	--------
*/

// Standard C
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// iota
#include "iota/strings.hh"


#pragma exceptions off


#define ANSI_SETCURSOR_HOME  "\x1b[" "H"
#define ANSI_CLEARSCREEN     "\x1b[" "2J"


int main( int argc, char *argv[] )
{
	const char* window_path = getenv( "WINDOW" );
	
	if ( window_path == NULL )
	{
		write( STDOUT_FILENO, STR_LEN( ANSI_CLEARSCREEN ANSI_SETCURSOR_HOME ) );
	}
	else
	{
		int window_dirfd = open( window_path, O_RDONLY | O_DIRECTORY );
		
		if ( window_dirfd < 0 )
		{
			return 1;
		}
		
		const char* text_subpath = "view/v/v/text";
		
		int text_fd = openat( window_dirfd, text_subpath, O_WRONLY | O_TRUNC );
		
		if ( text_fd < 0 )
		{
			return 1;
		}
	}
	
	return 0;
}

