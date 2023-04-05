/*
	base64-decode.cc
	----------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// more-posix
#include "more/perror.hh"

// plus
#include "plus/var_string.hh"

// vxo
#include "vxo/error.hh"

// vxs
#include "vxs/lib/base64_decode.hh"


#pragma exceptions off


#define PROGRAM  "base64-decode"


static inline
void report_error( const char* msg )
{
	more::perror( PROGRAM, msg );
}

static
void write_ln( int fd, plus::var_string& s )
{
	char*  p = &s[ 0 ];
	size_t n = s.size();
	
	char c = p[ n ];
	
	p[ n ] = '\n';
	
	write( STDOUT_FILENO, p, n + 1 );
	
	p[ n ] = c;
}

int main( int argc, char** argv )
{
	char** argp = argv + 1;
	
	bool failed = false;
	
	while ( *argp )
	{
		const char* input = *argp++;
		
		vxo::Box box = vxo::base64_decode( input, strlen( input ) );
		
		if ( const vxo::Error* error = box.is< vxo::Error >() )
		{
			report_error( error->get() );
			
			failed = true;
			continue;
		}
		
		write_ln( STDOUT_FILENO, *(plus::var_string*) &box );
	}
	
	return failed;
}
