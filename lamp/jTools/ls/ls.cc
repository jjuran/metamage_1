/*	=====
 *	ls.cc
 *	=====
 */

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include <errno.h>

// POSIX
#include <dirent.h>
#include <sys/stat.h>

// plus
#include "plus/var_string.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	static void iterate_dir( const char* pathname )
	{
		DIR* iter = opendir( pathname );
		
		while ( const dirent* ent = readdir( iter ) )
		{
			std::printf( "%s\n", ent->d_name );
		}
		
		closedir( iter );
	}
	
	int Main( int argc, iota::argv_t argv )
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
					
					plus::var_string message = "ls: ";
					
					message += argv[ i ];
					
					std::perror( message.c_str() );
					
					continue;
				}
				
				if ( S_ISDIR( st.st_mode ) )
				{
					iterate_dir( argv[ i ] );
				}
				else
				{
					std::printf( "%s\n", argv[ i ] );
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
	
}

