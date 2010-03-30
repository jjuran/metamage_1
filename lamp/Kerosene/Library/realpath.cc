/*
	realpath.cc
	-----------
*/

// Standard C
#include "errno.h"
#include "stdlib.h"

// POSIX
#include "unistd.h"


// Exceptions are off here
#pragma exceptions off


char* realpath( const char *path, char *buffer )
{
	const size_t buffer_size = 4096;
	
	const int saved_errno = errno;
	
	ssize_t length = realpath_k( path, buffer, buffer_size - 1 );
	
	if ( length == -1 )
	{
		return NULL;
	}
	
	if ( length < -1 )
	{
		errno = saved_errno;  // was ERANGE
		
		length = buffer_size - 1;
	}
	
	buffer[ length ] = '\0';
	
	return buffer;
}

