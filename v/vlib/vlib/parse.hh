/*
	parse.hh
	--------
*/

#ifndef VLIB_PARSE_HH
#define VLIB_PARSE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value parse( const char* p, const char* file );
	
}

#endif
