/*
	truncate.hh
	-----------
*/

#ifndef RELIX_SYSCALL_TRUNCATE_HH
#define RELIX_SYSCALL_TRUNCATE_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	int truncate( const char* path, off_t length );
	
}

#endif

