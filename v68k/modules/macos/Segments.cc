/*
	Segments.cc
	-----------
*/

#include "Segments.hh"

// POSIX
#include <unistd.h>


pascal void ExitToShell_patch()
{
	_exit( 0 );
}
