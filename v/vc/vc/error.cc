/*
	error.cc
	--------
*/

#include "vc/error.hh"

// POSIX
#include <stdlib.h>
#include <unistd.h>

// must
#include "must/write.h"


namespace vc
{
	
	void error( const char* msg, unsigned len )
	{
		must_write( STDOUT_FILENO, msg, len );
		exit( 1 );
	}
	
}
