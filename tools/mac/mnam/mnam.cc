/*
	mnam.cc
	-------
*/

// POSIX
#include <unistd.h>

// more-libc
#include "more/string.h"

// mac-sys-utils
#include "mac_sys/get_machine_name.hh"


int main( int argc, char** argv )
{
	const unsigned char* mnam = mac::sys::get_machine_name();
	
	if ( mnam )
	{
		const size_t length = mnam[ 0 ];
		
		char buffer[ 256 ];
		
		char* const end = (char*) mempcpy( buffer, &mnam[ 1 ], length );
		
		*end = '\n';
		
		ssize_t n = write( STDOUT_FILENO, buffer, length + 1 );
		
		if ( n != length + 1 )
		{
			return 13;
		}
		
		return 0;
	}
	
	return 1;
}
