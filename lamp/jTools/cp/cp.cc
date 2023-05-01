/*	=====
 *	cp.cc
 *	=====
 */

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <string.h>

// relix
#include "relix/copyfile.h"
#include "relix/pump.h"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/stat.hh"
#include "poseven/types/errno_t.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{

namespace p7 = poseven;


static const char* Basename( const char* pathname )
{
	const char* name_start = pathname;
	
	const char* p = pathname;
	
	while ( *p != '\0' )
	{
		if ( *p++ == '/' )
		{
			name_start = p;
		}
	}
	
	return name_start;
}

static int copyfile_or_pump( const char* src, const char* dest )
{
	int ok = ::copyfile( src, dest );
	
	if ( ok < 0  &&  (errno == EINVAL  ||  errno == EXDEV) )
	{
		int in = ok = open( src, O_RDONLY );
		
		if ( in >= 0 )
		{
			int out = ok = open( dest, O_WRONLY|O_CREAT, 0666 );
			
			if ( out >= 0 )
			{
				ssize_t pumped = pump( in, NULL, out, NULL, 0, 0 );
				
				pumped = pump( in, NULL, out, NULL, 0, 0 );
				
				ok = -(pumped != 0);
				
				close( out );
			}
			
			close( in );
		}
	}
	
	return ok;
}

int Main( int argc, char** argv )
{
	int fail = 0;
	
	// Check for sufficient number of args
	if ( argc < 3 )
	{
		fprintf( stderr, "cp: missing %s\n", (argc == 1) ? "file arguments"
		                                                 : "destination file" );
		
		return 1;
	}
	
	struct stat sb;
	
	if ( argc > 3 )
	{
		// Copy multiple files.
		
		// Last arg should be the destination directory.
		const char* destDir = argv[ argc - 1 ];
		
		p7::stat( destDir, sb );
		
		if ( bool not_a_dir = (sb.st_mode & S_IFDIR) == 0 )
		{
			fprintf( stderr, "cp: copying multiple files, but last argument (%s) is not a directory.\n",
			                                                                 destDir );
			
			return 1;
		}
		
		// Try to copy each file.  Return whether any errors occurred.
		for ( size_t index = 1;  index < argc - 1;  ++index )
		{
			const char* sourcePath = argv[index];
			
			plus::var_string destFilePath = destDir;
			
			if ( destFilePath.back() != '/' )
			{
				destFilePath += '/';
			}
			
			destFilePath += Basename( sourcePath );
			
			if ( -1 == copyfile_or_pump( sourcePath, destFilePath.c_str() ) )
			{
				++fail;
			}
		}
	}
	else
	{
		// Copy single file.
		const char* sourcePath = argv[1];
		const char* destPath   = argv[2];
		
		if ( -1 == lstat( sourcePath, &sb ) )
		{
			fprintf( stderr, "cp: %s: %s\n", sourcePath, strerror( errno ) );
			
			return 1;
		}
		
		plus::var_string destFilePath = destPath;
		
		if ( p7::stat( destPath, sb ) )
		{
			// dest exists
			
			bool isDir = sb.st_mode & S_IFDIR;
			
			if ( isDir )
			{
				// dest is a dir.
				// Copy this -> that/this
				// set that = that/this
				
				if ( destFilePath.back() != '/' )
				{
					destFilePath += '/';
				}
				
				destFilePath += Basename( sourcePath );
				
				destPath = destFilePath.c_str();
			}
		}
		
		p7::throw_posix_result( copyfile_or_pump( sourcePath, destPath ) );
	}
	
	return fail;
}

}
