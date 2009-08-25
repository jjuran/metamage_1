/*	=====
 *	ln.cc
 *	=====
 */

// Standard C
#include <errno.h>
#include <stdlib.h>

// Standard C/C++
#include <cstdio>
#include <cstring>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/Pathnames.hh"
#include "POSeven/functions/stat.hh"
#include "POSeven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static const char* Basename( const char* pathname )
	{
		std::size_t length = std::strlen( pathname );
		
		const char* end = pathname + length;
		
		for ( const char* p = end - 1;  p >= pathname;  --p )
		{
			if ( *p == '/' )
			{
				return p + 1;
			}
		}
		
		return pathname;
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		// Check for sufficient number of args
		if ( argc < 3 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: ln [-s] target [link]\n" ) );
			
			return 2;
		}
		
		const bool symbolic = std::strcmp( argv[ 1 ], "-s" ) == 0;
		
		const char* target = argv[ 1 + symbolic ];
		const char* loc    = argv[ 2 + symbolic ];
		
		std::string location = loc != NULL ? loc : ".";
		
		struct stat location_status;
		
		if ( bool location_exists = p7::stat( location, location_status ) )
		{
			if ( S_ISDIR( location_status.st_mode ) )
			{
				using namespace io::path_descent_operators;
				
				location = location / Basename( target );
				
				location_exists = p7::stat( location, location_status );
			}
			
			if ( location_exists )
			{
				std::fprintf( stderr, "ln: %s: %s\n", location.c_str(), std::strerror( EEXIST ) );
				
				return EXIT_FAILURE;
			}
		}
		
		int linked = symbolic ? symlink( target, location.c_str() )
		                      :    link( target, location.c_str() );
		
		if ( linked < 0 )
		{
			std::perror( "ln" );
			
			return EXIT_FAILURE;
		}
		
		return EXIT_SUCCESS;
	}

}

