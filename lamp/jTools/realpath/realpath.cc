/*	===========
 *	realpath.cc
 *	===========
 */

// Standard C++
#include <algorithm>
#include <string>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"


namespace poseven
{
	
	static std::string realpath( const char* pathname )
	{
		std::string result;
		
		size_t size = 128;
		
		while ( size > result.size() )
		{
			result.resize( size );
			
			size = realpath_k( pathname, &result[0], result.size() );
			
			if ( size < 0 )
			{
				std::perror( "realpath" );
				
				std::exit( EXIT_FAILURE );
			}
		}
		
		result.resize( size );
		
		return result;
	}
	
}


#pragma export on

int main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: realpath <file>\n" ) );
		
		return 1;
	}
	
	std::string resolved_path = poseven::realpath( argv[1] );
	
	resolved_path += "\n";
	
	(void) write( STDOUT_FILENO, resolved_path.data(), resolved_path.size() );
	
	return EXIT_SUCCESS;
}

#pragma export reset

