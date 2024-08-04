/*
	enough_string.cc
	----------------
*/

#include "recall/enough_string.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"


#pragma exceptions off


namespace recall
{

void enough_string::operator=( const char* s )
{
	assign( s, strlen( s ) );
}

void enough_string::operator+=( const char* s )
{
	append( s, strlen( s ) );
}

void enough_string::assign( const char* p, unsigned n )
{
	its_marker = its_buffer;
	
	append( p, n );
}

void enough_string::append( const char* p, unsigned n )
{
	bool overrun = n > its_limit - its_marker;
	
	if ( overrun )
	{
		n = its_limit - its_marker;
	}
	
	its_marker = (char*) mempcpy( its_marker, p, n );
	
	if ( overrun )
	{
		char* q = its_marker;
		
		*--q = '.';
		*--q = '.';
		*--q = '.';
	}
}

}
