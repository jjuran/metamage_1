/*	======
 *	pwd.cc
 *	======
 */

// Standard C++
#include <string>

// POSIX
#include <unistd.h>


int main( int argc, char const *const argv[] )
{
	std::string path;
	
	ssize_t size = 256;  // reasonable default
	
	// Even though we know the size after one iteration, it's theoretically possible
	// that the cwd has moved and now has a longer pathname.
	do
	{
		path.resize( size );
		
		size = getcwd_k( &path[ 0 ], path.size() );
	}
	while ( size > path.size() );
	
	path.resize( size );
	
	path += "\n";
	
	write( STDOUT_FILENO, path.data(), path.size() );
	
	return 0;
}

