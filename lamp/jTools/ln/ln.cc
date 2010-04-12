/*	=====
 *	ln.cc
 *	=====
 */

// Standard C
#include <errno.h>
#include <stdlib.h>

// Standard C/C++
#include <cstring>

// POSIX
#include <unistd.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/fstatat.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"

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
		
		int dirfd = AT_FDCWD;
		
		if ( loc != NULL )
		{
			dirfd = open( loc, O_RDONLY | O_DIRECTORY );
			
			bool failed = true;
			
			if ( dirfd >= 0 )
			{
				// It's a directory and therefore it exists.
				// See if a file with the target's name exists within it.
				
				const char* dir_path = loc;
				
				loc = Basename( target );
				
				struct stat sb;
				
				const bool exists = p7::fstatat( p7::fd_t( dirfd ), loc, sb );
				
				if (( failed = exists ))
				{
					// destination is a directory but the target's name is taken
					errno = EEXIST;
					
					const size_t dir_len = strlen( dir_path );
					const size_t loc_len = strlen( loc      );
					
					const size_t path_size = dir_len + 1 + loc_len;
					
					char* buffer = (char*) alloca( path_size + 1 );
					
					memcpy( buffer,               dir_path, dir_len );
					memcpy( buffer + dir_len,     STR_LEN( "/" )    );
					memcpy( buffer + dir_len + 1, loc,      loc_len );
					
					buffer[ path_size ] = '\0';
					
					loc = buffer;
				}
			}
			else if ( errno == ENOENT )
			{
				// okay, destination doesn't exist
				failed = false;
				
				dirfd = AT_FDCWD;
			}
			else if ( errno == ENOTDIR )
			{
				// destination exists but is not a directory
				errno = EEXIST;
			}
			
			if ( failed )
			{
				p7::perror( "ln", loc );
				
				return EXIT_FAILURE;
			}
		}
		
		int linked = symbolic ? symlinkat(           target, dirfd, loc    )
		                      :    linkat( AT_FDCWD, target, dirfd, loc, 0 );
		
		if ( linked < 0 )
		{
			p7::perror( "ln", target );
			
			return EXIT_FAILURE;
		}
		
		return EXIT_SUCCESS;
	}

}

