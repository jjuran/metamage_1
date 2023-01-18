/*	========
 *	lf2cr.cc
 *	========
 */

// Standard C
#include <errno.h>

// Standard C++
#include <algorithm>

// POSIX
#include <unistd.h>


#pragma exceptions off


int main( int argc, const char *const argv[] )
{
	while ( true )
	{
		enum { blockSize = 4096 };
		
		char data[ blockSize ];
		
		int bytes_read = read( STDIN_FILENO, data, blockSize );
		
		if ( bytes_read == 0 )
		{
			break;  // EOF
		}
		else if ( bytes_read == -1 )
		{
			return 1;
		}
		
		std::replace( data, data + bytes_read, '\n', '\r' );
		
		int bytes_written = write( STDOUT_FILENO, data, bytes_read );
		
		if ( bytes_written == -1 )
		{
			return 2;
		}
	}
	
	return 0;
}
