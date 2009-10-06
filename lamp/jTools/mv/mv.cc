/*	=====
 *	mv.cc
 *	=====
 */

// Standard C++
#include <string>

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include <errno.h>

// POSIX
#include <sys/stat.h>
#include <unistd.h>

// poseven
#include "poseven/Pathnames.hh"
#include "poseven/functions/stat.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static const char* basename( const char* path )
	{
		const char* slash = std::strrchr( path, '/' );
		
		if ( slash == NULL )
		{
			return path;
		}
		
		return slash + 1;
	}
	
	static int move_into_dir( const char* old_path, const char* new_dir )
	{
		using namespace io::path_descent_operators;
		
		const char* name = basename( old_path );
		
		std::string new_path = std::string( new_dir ) / name;
		
		return std::rename( old_path, new_path.c_str() );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		// Check for sufficient number of args
		if (argc < 3)
		{
			const char* prerequisite = (argc == 1) ? "file arguments" : "destination file";
			
			std::fprintf( stderr, "%s: missing %s\n", argv[0], prerequisite );
			
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
				const char* format = "%s: moving multiple files, but last argument (%s) is not a directory.\n";
				
				std::fprintf( stderr, format, argv[ 0 ], destDir );
				
				return 1;
			}
			
			for ( int index = 1;  index < argc - 1;  ++index )
			{
				fail += move_into_dir( argv[ index ], destDir ) == -1;
			}
		}
		else
		{
			// Move single file or directory.
			
			const char* srcPath  = argv[ 1 ];
			const char* destPath = argv[ 2 ];
			
			int renamed = io::directory_exists( destPath ) ? move_into_dir( srcPath, destPath )
			                                               : std::rename  ( srcPath, destPath );
			
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

}

