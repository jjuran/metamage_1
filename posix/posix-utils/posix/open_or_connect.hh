/*
	open_or_connect.hh
	------------------
*/

#ifndef POSIX_OPENORCONNECT_HH
#define POSIX_OPENORCONNECT_HH

// POSIX
#include <sys/types.h>


namespace posix
{
	
	int open_or_connect( const char* path, int flags, mode_t mode );
	
}

#endif
