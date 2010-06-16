/*	======
 *	env.cc
 *	======
 */

// Standard C/C++
#include <cstring>

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>

// plus
#include "plus/var_string.hh"


extern "C" char** environ;


static void DumpEnvironment()
{
	plus::var_string output;
	
	for ( char** envp = environ;  *envp != NULL;  ++envp )
	{
		output += *envp;
		output += "\n";
	}
	
	(void) write( STDOUT_FILENO, output.data(), output.size() );
}

int main( int argc, char** argv )
{
	while ( *++argv != NULL  &&  std::strchr( *argv, '=' ) )
	{
		putenv( *argv );
	}
	
	if ( *argv == NULL )
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

