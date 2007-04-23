/*	======
 *	env.cc
 *	======
 */

// Standard C++
#include <string>

// POSIX
#include <unistd.h>


static void DumpEnvironment()
{
	std::string output;
	
	for ( const char* const* envp = environ;  *envp != NULL;  ++envp )
	{
		output += *envp;
		output += "\n";
	}
	
	(void) write( STDOUT_FILENO, output.data(), output.size() );
}

#pragma export on

int main( int argc, char const *const argv[] )
{
	if ( argc == 1  &&  environ != NULL )
	{
		DumpEnvironment();
	}
	
	return 0;
}

#pragma export reset

