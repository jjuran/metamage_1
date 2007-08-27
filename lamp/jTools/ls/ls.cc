/*	=====
 *	ls.cc
 *	=====
 */

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstring>

// POSIX
#include <dirent.h>
#include <sys/stat.h>

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace O = Orion;


static void iterate_dir( const char* pathname )
{
	DIR* iter = opendir( pathname );
	
	while ( const dirent* ent = readdir( iter ) )
	{
		Io::Out << ent->d_name << "\n";
	}
	
	closedir( iter );
}

int O::Main( int argc, argv_t argv )
{
	if ( argc > 1 )
	{
		int fail = 0;
		
		for ( int i = 1; i < argc; i++ )
		{
			struct stat st;
			
			int result = stat( argv[ i ], &st );
			
			if ( result == -1 )
			{
				++fail;
				int errnum = errno;
				Io::Err << "failed to stat( " << argv[ i ] << " ), " << std::strerror( errnum ) << "\n";
				continue;
			}
			
			if ( S_ISDIR( st.st_mode ) )
			{
				iterate_dir( argv[ i ] );
			}
			else
			{
				Io::Out << argv[ i ] << "\n";
			}
		}
		return fail;
	}
	else
	{
		iterate_dir( "." );
	}
	
	return 0;
}

