/*	==========
 *	striplf.cc
 *	==========
 */

// Standard C
#include <errno.h>

// Standard C++
#include <algorithm>

// POSIX
#include <unistd.h>

// poseven
#include "poseven/functions/write.hh"


namespace p7 = poseven;


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
		
		if ( bytes_read == -1 )
		{
			return 1;
		}
		
		const char* p   = data;
		const char* end = data + bytes_read;
		
		const char* lf;
		
		while ( (lf = std::find( p, end, '\n' )) != end )
		{
			(void) p7::write( p7::stdout_fileno, p, lf - p );
			
			p = lf + 1;
		}
		
		(void) p7::write( p7::stdout_fileno, p, end - p );
	}
	
	return 0;
}

