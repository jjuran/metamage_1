/*	==========
 *	lf2crlf.cc
 *	==========
 */

// Standard C++
#include <algorithm>

// POSIX
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


static ssize_t checked_read( int fd, char* buffer, size_t length )
{
	ssize_t n_read = read( fd, buffer, length );
	
	if ( n_read < 0 )
	{
		more::perror( "lf2crlf", "read" );
		
		_exit( 1 );
	}
	
	return n_read;
}

static void checked_write( int fd, const char* buffer, size_t length )
{
	ssize_t n_written = write( fd, buffer, length );
	
	if ( n_written < length )
	{
		more::perror( "lf2crlf", "write" );
		
		_exit( 2 );
	}
}

int main( int argc, const char *const argv[] )
{
	while ( true )
	{
		enum { blockSize = 4096 };
		
		char data[ blockSize ];
		
		ssize_t bytes_read = checked_read( STDIN_FILENO, data, blockSize );
		
		if ( bytes_read == 0 )
		{
			break;  // EOF
		}
		
		const char* p   = data;
		const char* end = data + bytes_read;
		
		const char* lf;
		
		while ( (lf = std::find( p, end, '\n' )) != end )
		{
			checked_write( STDOUT_FILENO, p, lf - p         );
			checked_write( STDOUT_FILENO, STR_LEN( "\r\n" ) );
			
			p = lf + 1;
		}
		
		checked_write( STDOUT_FILENO, p, end - p );
	}
	
	return 0;
}
