/*	==========
 *	gestalt.cc
 *	==========
 */

// Standard C/C++
#include <cstdio>
#include <cstring>

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


int main( int argc, char const *const argv[] )
{
	int gestalt = open( "/sys/mac/gestalt", O_RDONLY, 0 );
	
	if ( gestalt == -1 )
	{
		std::perror( "gestalt: open" );
		
		return 1;
	}
	
	for ( ++argv;  *argv != NULL;  ++argv )
	{
		if ( std::strlen( *argv ) != 4 )
		{
			std::fprintf( stderr, "gestalt: expecting four-character code at '%s'\n", *argv );
		}
		
		int request = 0;
		
		std::memcpy( &request, *argv, 4 );
		
		int io = ioctl( gestalt, request, &request );
		
		if ( io == -1 )
		{
			std::perror( "gestalt: ioctl" );
			
			continue;
		}
		
		std::printf( "Gestalt( '%s' ):  0x%x, %d\n", *argv, request, request );
	}
	
	return 0;
}

