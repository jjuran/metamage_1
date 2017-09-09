/*
	analyze.hh
	----------
*/

#ifndef VLIB_ANALYZE_HH
#define VLIB_ANALYZE_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	class lexical_scope;
	
	Value analyze( const Value& syntree, lexical_scope* globals );
	
}

#endif
