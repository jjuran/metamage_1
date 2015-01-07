/*
	Segments.cc
	-----------
*/

#include "Segments.hh"

// POSIX
#include <unistd.h>

// macos
#include "options.hh"


pascal void ExitToShell_patch()
{
	if ( linger_on_exit )
	{
		pause();
	}
	
	_exit( 0 );
}
