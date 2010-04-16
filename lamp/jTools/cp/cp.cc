/*	=====
 *	cp.cc
 *	=====
 */

// Standard C++
#include <string>

// Standard C/C++
#include <cstdio>
#include <cstdlib>
#include <cstring>

// POSIX
#include <errno.h>
#include <unistd.h>

// poseven
#include "poseven/extras/copyfile.hh"
#include "poseven/functions/stat.hh"

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
	
	
	int Main( int argc, iota::argv_t argv )
	{
		int fail = 0;
		
		// Check for sufficient number of args
		if ( argc < 3 )
		{
			std::fprintf( stderr, "cp: missing %s\n", (argc == 1) ? "file arguments"
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
			
			if ( bool not_a_dir = sb.st_mode & S_IFDIR == 0 )
			{
				std::fprintf( stderr, "cp: copying multiple files, but last argument (%s) is not a directory.\n",
				                                                                      destDir );
				
				return EXIT_FAILURE;
			}
			
			// Try to copy each file.  Return whether any errors occurred.
			for ( std::size_t index = 1;  index < argc - 1;  ++index )
			{
				const char* sourcePath = argv[index];
				
				std::string destFilePath = destDir;
				
				if ( *(destFilePath.end() - 1) != '/' )
				{
					destFilePath += '/';
				}
				
				destFilePath += Basename( sourcePath );
				
				if ( -1 == copyfile( sourcePath, destFilePath.c_str() ) )
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
			
			if ( -1 == stat( sourcePath, &sb ) )
			{
				std::fprintf( stderr, "cp: cannot stat `%s': %s\n", sourcePath, strerror( errno ) );
				
				return 1;
			}
			
			std::string destFilePath = destPath;
			
			if ( p7::stat( destPath, sb ) )
			{
				// dest exists
				
				bool isDir = sb.st_mode & S_IFDIR;
				
				if ( isDir )
				{
					// dest is a dir.
					// Copy this -> that/this
					// set that = that/this
					
					if ( *(destFilePath.end() - 1) != '/' )
					{
						destFilePath += '/';
					}
					
					destFilePath += Basename( sourcePath );
					
					destPath = destFilePath.c_str();
				}
			}
			
			p7::copyfile( sourcePath, destPath );
		}
		
		return fail;
	}

}

