/*	=====
 *	mv.cc
 *	=====
 */

// Standard C
#include "errno.h"

// Standard C/C++
#include <cstdio>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


int O::Main( int argc, const char *const argv[] )
{
	// Check for sufficient number of args
	if (argc < 3)
	{
		const char* prerequisite = (argc == 1) ? "file arguments" : "destination file";
		
		Io::Err << "mv: missing " << prerequisite << "\n";
		
		return 1;
	}
	
	int fail = 0;
	
	if ( argc > 3 )
	{
		// Move multiple files.
		
		const char* destDir = argv[ argc - 1 ];
		
		struct stat sb;
		int statted = stat( destDir, &sb );
		
		if ( statted == -1 )
		{
			std::fprintf( stderr, "Can't stat %s (%d)\n", destDir, errno );
			return 1;
		}
		
		if ( !S_ISDIR( sb.st_mode ) )
		{
			//Io::Err << "mv: moving multiple files, but last argument (" << argv[argc - 1] << ") is not a directory.\n";
			const char* format = "%s: moving multiple files, but last argument (%s) is not a directory.\n";
			
			std::fprintf( stderr, format, argv[ 0 ], destDir );
			
			return 1;
		}
		
		for ( int index = 1;  index < argc - 1;  ++index )
		{
			fail += rename( argv[ index ], destDir ) == -1;
		}
	}
	else
	{
		// Move single file or directory.
		
		const char* srcPath  = argv[ 1 ];
		const char* destPath = argv[ 2 ];
		
		int renamed = rename( srcPath, destPath );
		
		if ( renamed == -1 )
		{
			int err = errno;
			
			const char* msg = "unchecked error";
			
			switch ( errno )
			{
				case ENOENT:
					msg = "No such file or directory";
					break;
				
				case EXDEV:
					msg = "can't move across partitions";
					break;
				
				default:
					std::fprintf( stderr, "Errno = %d\n", errno );
					break;
			}
			
			std::fprintf( stderr, "%s: rename %s to %s: %s\n", argv[ 0 ], srcPath, destPath, msg );
			
			return 1;
		}
		
		return 0;
	}
	
	return fail != 0;
}

