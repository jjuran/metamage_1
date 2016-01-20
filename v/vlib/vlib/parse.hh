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
	
	class lexical_scope;
	
	Value parse( const char* p, const char* file, lexical_scope* globals );
	
}

#endif
