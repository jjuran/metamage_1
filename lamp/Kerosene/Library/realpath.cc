/*
	realpath.cc
	-----------
*/

// Standard C
#include "errno.h"
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "unistd.h"


// Exceptions are off here
#pragma exceptions off


ssize_t _realpath( const char *path, char *buffer, size_t buffer_size )
{
	return _realpathat( AT_FDCWD, path, buffer, buffer_size, 0 );
}

char* realpath( const char *path, char *buffer )
{
	if ( buffer == NULL )
	{
		ssize_t length = _realpath( path, buffer, 0 );
		
		length = -length;
		
		if (( buffer = (char*) malloc( length ) ))
		{
			length = _realpath( path, buffer, length - 1 );
			
			if ( length < 0 )
			{
				free( buffer );
				return NULL;
			}
			
			buffer[ length ] = '\0';
		}
		
		return buffer;
	}
	
	const size_t buffer_size = 4096;
	
	const int saved_errno = errno;
	
	ssize_t length = _realpath( path, buffer, buffer_size - 1 );
	
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
