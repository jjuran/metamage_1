/*	======
 *	env.cc
 *	======
 */

// Standard C++
#include <string>

// Standard C
#include <stdlib.h>

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
	while ( *++argv != NULL  &&  std::strchr( *argv, '=' ) )
	{
		putenv( *argv );
	}
	
	if ( argv == NULL )
	{
		DumpEnvironment();
	}
	else
	{
		execvp( argv[0], argv );
		
		_exit( 127 );
	}
	
	return 0;
}

#pragma export reset

