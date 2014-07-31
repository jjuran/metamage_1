/*
	errors.cc
	---------
*/

#include "command/errors.hh"

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// Standard C
#include <stdlib.h>
#include <string.h>


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ARRAYLEN( array )  (sizeof array / sizeof array[0])


namespace command
{
	
	void usage( const char* text, unsigned text_size, const char* arg )
	{
		const iovec iov[] =
		{
			{ (void*) text, text_size    },
			{ (void*) arg, strlen( arg ) },
			{ (void*) STR_LEN( "\n" )    },
		};
	
		writev( STDERR_FILENO, iov, ARRAYLEN( iov ) );
	
		exit( 2 );
	}
	
}
