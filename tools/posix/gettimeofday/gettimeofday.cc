/*
	gettimeofday.cc
	---------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// gear
#include "gear/inscribe_decimal.hh"


int main()
{
	timeval t;
	
	int got = gettimeofday( &t, NULL );
	
	if ( got < 0 )
	{
		return 1;  // shouldn't happen
	}
	
	char* s = gear::inscribe_decimal( t.tv_sec );  // FIXME:  Y2038 problem
	
	size_t len = strlen( s );
	
	s[ len++ ] = '\n';
	
	write( STDOUT_FILENO, s, len );
	
	return 0;
}

