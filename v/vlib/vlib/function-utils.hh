/*
	function-utils.hh
	-----------------
*/

#ifndef VLIB_FUNCTIONUTILS_HH
#define VLIB_FUNCTIONUTILS_HH

// vlib
#include "vlib/value.hh"


namespace vlib
{
	
	Value call_function( const Value& f, const Value& arguments );
	
}

#endif
