/*	===========
 *	readlink.cc
 *	===========
 */

// POSIX
#include "unistd.h"

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Errno.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace P7 = POSeven;
namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		Io::Err << "usage: readlink <file>\n";
		
		return 1;
	}
	
	const std::size_t path_len = 1024;
	
	char target_path[ path_len ];
	
	int result = readlink( argv[1], target_path, path_len );
	
	if ( result == -1 )
	{
		return 1;  // No error output
	}
	
	ASSERT( result > 0 );
	
	Io::Out << std::string( target_path, result ) << "\n";
	
	return 0;
}

