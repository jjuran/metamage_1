/*
	concat.hh
	---------
	
	Copyright 2010, Joshua Juran
*/

#ifndef PLUS_CONCAT_HH
#define PLUS_CONCAT_HH

// plus
#include "plus/string.hh"


namespace plus
{
	
	string concat( const char*  a, string::size_type  a_size,
	               const char*  b, string::size_type  b_size );
	
}

#endif

