/*	=====
 *	ln.cc
 *	=====
 */

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstring>

// POSIX
#include <unistd.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// Relix
#include "relix/alloca.h"

// more-libc
#include "more/string.h"

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/functions/fstatat.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


#ifndef AT_LINK_ALIAS
#define AT_LINK_ALIAS  0
#endif

#if AT_LINK_ALIAS
#define LN_OPTIONS  "[-s | -a]"
#else
#define LN_OPTIONS  "[-s]"
#endif


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


int Main( int argc, char** argv )
{
	bool symbolic = false;
	bool aliased  = false;
	
	const bool has_option = argc > 1  &&  argv[1][0] == '-';
	
	// Check for sufficient number of args
	bool misused = argc < 3 + has_option;
	
	if ( has_option  &&  !misused )
	{
		const char c1 = argv[1][1];
		
		if ( AT_LINK_ALIAS  &&  c1 == '-' )
		{
			if ( strcmp( argv[1] + 3, "alias" ) == 0 )
			{
				aliased = true;
			}
			else
			{
				misused = true;
			}
		}
		else
		{
			if ( argv[1][2] != '\0' )
			{
				misused = true;
			}
			else if ( AT_LINK_ALIAS  &&  c1 == 'a' )
			{
				aliased = true;
			}
			else if ( c1 == 's' )
			{
				symbolic = true;
			}
			else
			{
				misused = true;
			}
		}
	}
	
	if (  misused )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "Usage: ln " LN_OPTIONS " target [link]\n" ) );
		
		return 2;
	}
	
	const char* target = argv[ 1 + has_option ];
	const char* loc    = argv[ 2 + has_option ];
	
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
				
				char* buffer = (char*) checked_alloca( path_size + 1 );
				
				char* p = buffer;
				
				p = (char*) mempcpy( p, dir_path, dir_len );
				*p++ = '/';
				mempcpy( p, loc, loc_len );
				
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
			
			return 1;
		}
	}
	
	const int flags = aliased * (AT_LINK_ALIAS | AT_SYMLINK_FOLLOW);
	
	int linked = symbolic ? symlinkat(           target, dirfd, loc        )
	                      :    linkat( AT_FDCWD, target, dirfd, loc, flags );
	
	if ( linked < 0 )
	{
		p7::perror( "ln", target );
		
		return 1;
	}
	
	return 0;
}

}
