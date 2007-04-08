/*	==========
 *	lf2crlf.cc
 *	==========
 */

// Standard C
#include <errno.h>

// Standard C++
#include <algorithm>

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/FileDescriptor.hh"


namespace P7 = POSeven;


#pragma export on

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
		
		const char CRLF[] = "\r\n";
		
		while ( (lf = std::find( p, end, '\n' )) != end )
		{
			(void) io::write( P7::kStdOut_FileNo, p,    lf - p          );
			(void) io::write( P7::kStdOut_FileNo, CRLF, sizeof CRLF - 1 );
			
			p = lf + 1;
		}
		
		(void) io::write( P7::kStdOut_FileNo, p, end - p );
	}
	
	return 0;
}

#pragma export reset

