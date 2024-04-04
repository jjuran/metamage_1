/*
	mv.cc
	-----
*/

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <string.h>

// Extended API Set Part 2
#include "extended-api-set/part-2.h"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


static
const char* Basename( const char* path )
{
	const char* slash = strrchr( path, '/' );
	
	if ( slash == NULL )
	{
		return path;
	}
	
	return slash + 1;
}

static
int move_into_dir( const char* old_path, int new_dirfd )
{
	const char* name = Basename( old_path );
	
	return renameat( AT_FDCWD, old_path, new_dirfd, name );
}

#define str_len( s )  s, strlen( s )

#define MOVING  "mv: moving multiple files, but last argument ("
#define NOT_DIR  ") is not a directory.\n"

int main( int argc, char const *const argv[] )
{
	// Check for sufficient number of args
	if ( argc < 3 )
	{
		if ( argc == 1 )
		{
			write( STDERR_FILENO, STR_LEN( "mv: missing file arguments\n" ) );
		}
		else
		{
			write( STDERR_FILENO, STR_LEN( "mv: missing destination file\n" ) );
		}
		
		return 1;
	}
	
	const char* destPath = argv[ argc - 1 ];
	
	int dest_dirfd = open( destPath, O_RDONLY | O_DIRECTORY );
	
	int fail = 0;
	
	if ( argc > 3 )
	{
		// Move multiple files.
		
		if ( dest_dirfd < 0 )
		{
			if ( errno == ENOTDIR )
			{
				write( STDERR_FILENO, STR_LEN( MOVING   ) );
				write( STDERR_FILENO, str_len( destPath ) );
				write( STDERR_FILENO, STR_LEN( NOT_DIR  ) );
			}
			else
			{
				const char* errmsg = strerror( errno );
				
				write( STDERR_FILENO, STR_LEN( "mv: "   ) );
				write( STDERR_FILENO, str_len( destPath ) );
				write( STDERR_FILENO, STR_LEN( ": "     ) );
				write( STDERR_FILENO, str_len( errmsg   ) );
				write( STDERR_FILENO, STR_LEN( "\n"     ) );
			}
			
			return 1;
		}
		
		for ( int i = 1;  i < argc - 1;  ++i )
		{
			fail += move_into_dir( argv[ i ], dest_dirfd ) < 0;
		}
	}
	else
	{
		// Move single file or directory.
		
		const char* srcPath  = argv[ 1 ];
		
		int renamed = dest_dirfd >= 0 ? move_into_dir( srcPath, dest_dirfd )
		                              : rename       ( srcPath, destPath   );
		
		if ( renamed < 0 )
		{
			int saved_errno = errno;
			
			const char* errmsg = errno == EXDEV ? "can't move across partitions"
			                                    : strerror( errno );
			
			write( STDERR_FILENO, STR_LEN( "mv: rename " ) );
			write( STDERR_FILENO, str_len( srcPath       ) );
			write( STDERR_FILENO, STR_LEN( " to "        ) );
			write( STDERR_FILENO, str_len( destPath      ) );
			write( STDERR_FILENO, STR_LEN( ": "          ) );
			write( STDERR_FILENO, str_len( errmsg        ) );
			write( STDERR_FILENO, STR_LEN( "\n"          ) );
			
			return 1;
		}
		
		return 0;
	}
	
	return fail != 0;
}
