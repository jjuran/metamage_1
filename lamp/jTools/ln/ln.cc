/*
	ln.cc
	-----
*/

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// Relix
#include "relix/alloca.h"

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"


#ifndef AT_LINK_ALIAS
#define AT_LINK_ALIAS  0
#endif

#if AT_LINK_ALIAS
#define LN_OPTIONS  "[-s | -a]"
#else
#define LN_OPTIONS  "[-s]"
#endif

#define STR_LEN( s )  "" s, (sizeof s - 1)


#pragma exceptions off


static
const char* basename( const char* pathname )
{
	size_t length = strlen( pathname );
	
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

int main( int argc, char** argv )
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
		write( STDERR_FILENO, STR_LEN( "Usage: ln " LN_OPTIONS " target [link]\n" ) );
		
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
			
			loc = basename( target );
			
			struct stat st;
			
			int nok = fstatat( dirfd, loc, &st, 0 );
			
			const bool exists = nok == 0;
			
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
			else if ( errno != ENOENT )
			{
				more::perror( "ln", target );
				
				return 1;
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
			more::perror( "ln", loc );
			
			return 1;
		}
	}
	
	const int flags = aliased * (AT_LINK_ALIAS | AT_SYMLINK_FOLLOW);
	
	int linked = symbolic ? symlinkat(           target, dirfd, loc        )
	                      :    linkat( AT_FDCWD, target, dirfd, loc, flags );
	
	if ( linked < 0 )
	{
		more::perror( "ln", target );
		
		return 1;
	}
	
	return 0;
}
