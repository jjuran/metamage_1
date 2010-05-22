/*	=====
 *	mv.cc
 *	=====
 */

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include <errno.h>
#include <stdio.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Extended API Set Part 2
#include "extended-api-set/part-2.h"


#pragma exceptions off


static const char* Basename( const char* path )
{
	const char* slash = std::strrchr( path, '/' );
	
	if ( slash == NULL )
	{
		return path;
	}
	
	return slash + 1;
}

static int move_into_dir( const char* old_path, int new_dirfd )
{
	const char* name = Basename( old_path );
	
	return ::renameat( AT_FDCWD, old_path, new_dirfd, name );
}

int main( int argc, char const *const argv[] )
{
	// Check for sufficient number of args
	if (argc < 3)
	{
		const char* prerequisite = (argc == 1) ? "file arguments" : "destination file";
		
		std::fprintf( stderr, "%s: missing %s\n", argv[0], prerequisite );
		
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
				const char* format = "%s: moving multiple files, but last argument (%s) is not a directory.\n";
				
				std::fprintf( stderr, format, argv[ 0 ], destPath );
			}
			else
			{
				std::fprintf( stderr, "mv: %s: %s\n", destPath, std::strerror( errno ) );
			}
			
			return 1;
		}
		
		for ( int index = 1;  index < argc - 1;  ++index )
		{
			fail += move_into_dir( argv[ index ], dest_dirfd ) == -1;
		}
	}
	else
	{
		// Move single file or directory.
		
		const char* srcPath  = argv[ 1 ];
		
		int renamed = dest_dirfd >= 0 ? move_into_dir( srcPath, dest_dirfd )
		                              : std::rename  ( srcPath, destPath   );
		
		if ( renamed == -1 )
		{
			const char* error_msg = errno == EXDEV ? "can't move across partitions" : std::strerror( errno );
			
			std::fprintf( stderr, "%s: rename %s to %s: %s\n", argv[ 0 ], srcPath, destPath, error_msg );
			
			return 1;
		}
		
		return 0;
	}
	
	return fail != 0;
}

