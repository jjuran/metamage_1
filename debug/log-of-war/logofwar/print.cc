/*
	print.cc
	--------
*/

#include "logofwar/print.hh"

// Standard C
#include <stdio.h>


namespace logofwar
{

void print( const char* s )
{
	fprintf( stderr, "%s", s );
}

void print_dec( int x )
{
	fprintf( stderr, "%d", x );
}

void print_hex( int x )
{
	fprintf( stderr, "%x", x );
}

}
