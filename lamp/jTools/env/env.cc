/*
	env.cc
	------
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// POSIX
#include <unistd.h>

// more-libc
#include "more/string.h"

// must
#include "must/write.h"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


extern "C" char** environ;


static void DumpEnvironment()
{
	size_t size = 0;
	
	for ( char** envp = environ;  *envp != NULL;  ++envp )
	{
		size += strlen( *envp ) + 1;
	}
	
	void* alloc = malloc( size );
	
	if ( alloc == NULL )
	{
		must_write( STDERR_FILENO, STR_LEN( "*** Out of memory! ***\n" ) );
		_exit( 1 );
	}
	
	char* p = (char*) alloc;
	
	for ( char** envp = environ;  *envp != NULL;  ++envp )
	{
		p = (char*) mempcpy( p, *envp, strlen( *envp ) );
		*p++ = '\n';
	}
	
	must_write( STDOUT_FILENO, alloc, size );
	
	free( alloc );
}

int main( int argc, char** argv )
{
	while ( *++argv != NULL  &&  strchr( *argv, '=' ) )
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
