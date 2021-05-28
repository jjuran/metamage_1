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

// more-libc
#include "more/string.h"

// must
#include "must/write.h"

// plus
#include "plus/string.hh"


extern "C" char** environ;


static void DumpEnvironment()
{
	size_t size = 0;
	
	for ( char** envp = environ;  *envp != NULL;  ++envp )
	{
		size += strlen( *envp ) + 1;
	}
	
	plus::string output;
	
	char* p = output.reset( size );
	
	for ( char** envp = environ;  *envp != NULL;  ++envp )
	{
		p = (char*) mempcpy( p, *envp, strlen( *envp ) );
		*p++ = '\n';
	}
	
	must_write( STDOUT_FILENO, output.data(), output.size() );
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
