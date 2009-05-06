/*	==========
 *	ttyname.cc
 *	==========
 */

// Standard C
#include <assert.h>
#include "errno.h"

// POSIX
#include "unistd.h"

// Iota
#include "iota/strings.hh"


// Exceptions are off here
#pragma exceptions off


static int decimal_magnitude( unsigned x )
{
	int result = 0;
	
	while ( x > 0 )
	{
		x /= 10;
		
		++result;
	}
	
	return result;
}

static char* inscribe_decimal_backwards( unsigned x, char* p )
{
	while ( x > 0 )
	{
		*--p = x % 10 + '0';
		
		x /= 10;
	}
	
	return p;
}

ssize_t ttyname_k( int fd, char* buffer, size_t buffer_size )
{
	if ( !isatty( fd ) )
	{
		//errno = ENOTTY;
		
		return -1;
	}
	
	char pathname[] = "/dev/fd/123456789";
	
	char* begin = pathname + STRLEN( "/dev/fd/" );
	
	char* end = begin + decimal_magnitude( fd );
	
	if ( begin < end )
	{
		inscribe_decimal_backwards( fd, end );
	}
	else
	{
		*end++ = '0';
	}
	
	*end = '\0';
	
	return readlink_k( pathname, buffer, buffer_size );
}

int ttyname_r( int fd, char* buffer, size_t buffer_size )
{
	ssize_t length = ttyname_k( fd, buffer, buffer_size - 1 );
	
	if ( length < 0 )
	{
		assert( errno != 0 );
		
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

