/*
	iota/string_traits.cc
	---------------------
	
	Copyright 2010, Joshua Juran
*/

#include "iota/string_traits.hh"

// Standard C
#include <string.h>


namespace iota
{
	
	unsigned long string_size< const char* >::get( const char* s )
	{
		return strlen( s );
	}
	
}

