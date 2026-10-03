/*
	documents.cc
	------------
*/

#include "frend/documents.hh"

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.2's headers.
#include <sys/types.h>
#endif
#include <dirent.h>

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// plus
#include "plus/var_string.hh"

// frontend-common
#include "frend/load_file.hh"


#define STRLEN( s )  (sizeof "" s - 1)
#define STR_LEN( s )  s, STRLEN( s )


namespace frend
{

/*
	Open and close the directory.  While this is a no-op as far as
	the file system is concerned (with the possible exception of
	inode access times), it's enough to trigger a folder permission
	check in macOS (which indeed is this function's very purpose).
*/

static
int scrub_dir( const char* path )
{
	if ( ! path )
	{
		return ENOENT;
	}
	
	if ( DIR* dir = opendir( path ) )
	{
		closedir( dir );
		
		return 0;
	}
	
	return errno;
}

static
plus::string load_config( int bindir_fd )
{
	const char* config_path = "../Resources/config.txt";
	
	plus::var_string config = load_file( bindir_fd, config_path );
	
	if ( ! config.empty() )
	{
		char* p = &config[ 0 ];
		
		while ( (p = strchr( p, '\n' )) )
		{
			*p++ = '\0';
		}
	}
	
	return config;
}

static
const char* make_docfs_path( const char* value )
{
	size_t n = strlen( value );
	
	if ( n <= 2  ||  value[ 0 ] != '"'  ||  value[ n - 1 ] != '"' )
	{
		return NULL;
	}
	
	static plus::var_string s;
	
	s.clear();
	
	value += 1;
	
	n -= 2;
	
	if ( strchr( value, '/' ) )
	{
		// see below
	}
	else if ( const char* home = getenv( "HOME" ) )
	{
		s.assign( home );
		
		s += "/Documents/Advanced Mac Substitute/";
	}
	else
	{
		return NULL;
	}
	
	s.append( value, n );
	
	return s.c_str();
}

static
const char* get_docfs_path( int bindir_fd )
{
	plus::string config = load_config( bindir_fd );
	
	if ( ! config.empty() )
	{
		const char* data = config.data();
		size_t      size = config.size();
		
		const char* end = data + size;
		
		while ( data < end )
		{
			size_t n = strlen( data );
			
			if ( strncmp( data, STR_LEN( "docfs: " ) ) == 0 )
			{
				const char* value = data + STRLEN( "docfs: " );
				
				return make_docfs_path( value );
			}
			
			data += n + 1;
		}
	}
	
	return NULL;
}

void scrub_documents_dir( int bindir_fd )
{
	const char* docfs_path = get_docfs_path( bindir_fd );
	
	scrub_dir( docfs_path );
}

}
