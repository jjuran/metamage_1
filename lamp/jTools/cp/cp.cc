/*	=====
 *	cp.cc
 *	=====
 */


// Standard C/C++
#include <cstring>

// POSIX
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace P7 = POSeven;
namespace O = Orion;


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


int O::Main(int argc, const char *const argv[])
{
	int fail = 0;
	
	// Check for sufficient number of args
	if ( argc < 3 )
	{
		Io::Err << "cp: missing " << ((argc == 1) ? "file arguments" : "destination file") << "\n";
		return 1;
	}
	
	struct stat sb;
	
	if ( argc > 3 )
	{
		// Copy multiple files.
		
		// Last arg should be the destination directory.
		const char* destDir = argv[ argc - 1 ];
		
		P7::ThrowPOSIXResult( stat( destDir, &sb ) );
		
		bool isDir = sb.st_mode & S_IFDIR;
		
		if ( !isDir )
		{
			Io::Err << "cp: copying multiple files, but last argument (" << destDir << ") is not a directory.\n";
			return 1;
		}
		
		// Try to copy each file.  Return whether any errors occurred.
		for ( std::size_t index = 1;  index < argc - 1;  ++index )
		{
			const char* sourcePath = argv[index];
			
			std::string destFilePath = destDir;
			
			if ( *destFilePath.rbegin() != '/' )
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
		
		if ( 0 == stat( destPath, &sb ) )
		{
			// dest exists
			
			bool isDir = sb.st_mode & S_IFDIR;
			
			if ( isDir )
			{
				// dest is a dir.
				// Copy this -> that/this
				// set that = that/this
				
				if ( *destFilePath.rbegin() != '/' )
				{
					destFilePath += '/';
				}
				
				destFilePath += Basename( sourcePath );
				
				destPath = destFilePath.c_str();
			}
		}
		else if ( errno != ENOENT )
		{
			P7::ThrowErrno( errno );
		}
		
		P7::ThrowPOSIXResult( copyfile( sourcePath, destPath ) );
	}
	
	return fail;
}

