/*	==========
 *	ttyname.cc
 *	==========
 */

// Standard C
#include "errno.h"

// POSIX
#include "unistd.h"

// Iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"

// Debug
#include "debug/assert.hh"


// Exceptions are off here
#pragma exceptions off


ssize_t ttyname_k( int fd, char* buffer, size_t buffer_size )
{
	if ( !isatty( fd ) )
	{
		//errno = ENOTTY;
		
		return -1;
	}
	
	char pathname[] = "/dev/fd/123456789";
	
	char* begin = pathname + STRLEN( "/dev/fd/" );
	
	char* end = gear::inscribe_unsigned_decimal_r( fd, begin );
	
	*end = '\0';
	
	return _readlink( pathname, buffer, buffer_size );
}

int ttyname_r( int fd, char* buffer, size_t buffer_size )
{
	ssize_t length = ttyname_k( fd, buffer, buffer_size - 1 );
	
	if ( length < 0 )
	{
		ASSERT( errno != 0 );
		
		return errno;
	}
	
	buffer[ length ] = '\0';
	
	return 0;
}

char* ttyname( int fd )
{
	static char buffer[ 256 ];  // should be enough for a terminal name
	
	ssize_t length = ttyname_k( fd, buffer, sizeof buffer - 1 );
	
	if ( length < 0 )
	{
		return NULL;
	}
	
	buffer[ length ] = '\0';
	
	return buffer;
}
