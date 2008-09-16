/*	=====
 *	ln.cc
 *	=====
 */

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstdio>
#include <cstring>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Pathnames.hh"
#include "POSeven/Stat.hh"

// Orion
#include "Orion/GetOptions.hh"
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
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: ln -s target [link]\n" ) );
			
			return 2;
		}
		
		if ( std::strcmp( argv[ 1 ], "-s" ) != 0 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: ln -s target [link]\n"
			                                       "(Sorry, symlinks only)\n" ) );
			
			return 2;
		}
		
		const char* target   = argv[ 2 ];
		std::string location = argv[ 3 ] != NULL ? argv[ 3 ] : ".";
		
		struct stat target_status = p7::stat( target );
		
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
		
		int linked = symlink( target, location.c_str() );
		
		if ( linked < 0 )
		{
			std::perror( "symlink()" );
			
			return EXIT_FAILURE;
		}
		
		return EXIT_SUCCESS;
	}

}

