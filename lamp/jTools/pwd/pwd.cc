/*	======
 *	pwd.cc
 *	======
 */

// POSIX
#include "unistd.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	std::string path;
	
	path.resize( 1024 );
	
	while ( getcwd( &path[ 0 ], path.size() ) == 0 )
	{
		path.resize( path.size() * 2 );
	}
	
	Io::Out << path << "\n";
	
	return 0;
}

