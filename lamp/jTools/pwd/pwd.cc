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
	
	ssize_t got = ~256;  // reasonable default
	
	// Even though we know the size after one iteration, it's theoretically possible
	// that the cwd has moved and now has a longer pathname.
	do
	{
		const size_t size = ~got;
		
		path.resize( size );
		
		got = getcwd_k( &path[ 0 ], path.size() );
	}
	while ( got < -1 );
	
	if ( got == -1 )
	{
		return EXIT_FAILURE;
	}
	
	path.resize( got );
	
	path += "\n";
	
	write( STDOUT_FILENO, path.data(), path.size() );
	
	return 0;
}

