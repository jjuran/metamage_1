/*
	demangle.cc
	-----------
*/

// POSIX
#include <unistd.h>

// recall
#include "recall/demangle.hh"

// plus
#include "plus/var_string.hh"


#pragma exceptions off


static
void demangle( const char* name )
{
	char buffer[ 512 ];
	
	recall::enough_string demangled( buffer, sizeof buffer );
	
	name[0] == '.' ? recall::demangle_MWCPPC( demangled, name )
	               : recall::demangle_MWC68K( demangled, name );
	
	demangled += "\n";
	
	write( STDOUT_FILENO, demangled.data(), demangled.size() );
}

int main( int argc, char** argv )
{
	while ( *++argv != NULL )
	{
		demangle( *argv );
	}
	
	return 0;
}
