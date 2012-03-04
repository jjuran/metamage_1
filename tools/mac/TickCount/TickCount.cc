/*
	TickCount.c
	-----------
*/

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"


int main( int argc, char** argv )
{
	if ( argc >= 1 + 1 )
	{
		const char* s = argv[ 1 ];
		
		sleep( gear::parse_unsigned_decimal( s ) );
	}
	
	char* ticks_str = gear::inscribe_unsigned_decimal( TickCount() );
	
	int len = strlen( ticks_str );
	
	ticks_str[ len++ ] = '\n';
	
	write( STDOUT_FILENO, ticks_str, len );
	
	return 0;
}
