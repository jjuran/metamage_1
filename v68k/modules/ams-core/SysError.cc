/*
	SysError.cc
	-----------
*/

#include "SysError.hh"

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


void SysError_patch( short error : __D0 )
{
	write( STDERR_FILENO, STR_LEN( "Sorry, a system error occurred.\n" ) );
	
	_exit( error );
}
