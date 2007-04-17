/*	======
 *	pwd.cc
 *	======
 */

// Standard C++
#include <string>

// POSIX
#include "unistd.h"


#pragma export on

int main( int /*argc*/, char const *const /*argv*/[] )
{
	std::string path;
	
	path.resize( 1024 );
	
	while ( getcwd( &path[ 0 ], path.size() ) == 0 )
	{
		path.resize( path.size() * 2 );
	}
	
	path += "\n";
	
	write( STDOUT_FILENO, path.data(), path.size() );
	
	return 0;
}

#pragma export reset

